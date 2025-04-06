#include "rtcservice.h"
using namespace rtc;
using namespace std;
using namespace Media;
Q_LOGGING_CATEGORY(LC_RTC_SERVICE, "RTCService");
uint32_t generate_ssrc()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 10000);
	return dist6(rng);
}
std::chrono::system_clock::rep time_since_epoch() {
	static_assert(
		std::is_integral<std::chrono::system_clock::rep>::value,
		"Representation of ticks isn't an integral value."
		);
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}
void Service::openLocalVideo(int userID,std::shared_ptr<FramePipe> input, Media::Video::SourceConfig config)
{
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getOrCreatePeerContext(userID);
	}
	std::scoped_lock lck{ _localVideo.mutex, ctx->video.mutex};
	if(!_localVideo.encoder)
	{
		_localVideo.encoder = std::make_shared<Media::Video::H264Encoder>();
	}
	if(_localVideo.encoder->input() != input)
		_localVideo.encoder->start(input, std::move(config));

	PacketizationConfig pConfig;
	pConfig.ecnCtx = _localVideo.encoder->codecContext();
	if(!_localVideo.packetizer)
	{
		_localVideo.packetizer = std::make_shared<Media::RtpPacketizer>(pConfig);
	}
	if (!_localVideo.packetizer->isStarted())
	{
		_localVideo.packetizer->start(_localVideo.encoder->output());
	}

	auto packetizerRawHandler = [this,userID,wCtx = std::weak_ptr(ctx)](std::shared_ptr<Media::Raw> raw, size_t index) {
		auto ctx = wCtx.lock();
		if (!ctx)
		{
			qCWarning(LC_RTC_SERVICE) << "Cannot send video packet, peer" << userID << "isReleased";
			_localVideo.packetizer->output()->unmapReading(index);
			return;
		}
		//std::scoped_lock lck{_localVideo.mutex, ctx->video.mutex};

		try {
			auto rtp = reinterpret_cast<rtc::RtpHeader*>(raw->raw);
			rtp->setSsrc(1);
			ctx->video.track->send((std::byte*)raw->raw, raw->size);
		}
		catch (std::exception& e)
		{
			qCWarning(LC_RTC_SERVICE) << "Cannot send video packet" << e.what();
		}
		_localVideo.packetizer->output()->unmapReading(index);
		};
	if (!ctx->video.track)
	{
		rtc::Description::Video media("video", rtc::Description::Direction::SendRecv);
		media.addH264Codec(96); // Must match the payload type of the external h264 RTP stream
		media.addSSRC(1, "video-stream");
		std::shared_ptr<PeerConnection> pc = ctx->pc;
		ctx->video.track = pc->addTrack(media);
		auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(1, "video-stream", 96, rtc::H264RtpPacketizer::defaultClockRate);
		auto videoReceivingSession = std::make_shared<rtc::H264RtpDepacketizer>(rtc::H264RtpDepacketizer::Separator::LongStartSequence);
		auto sess = std::make_shared<RtcpReceivingSession>();
		videoReceivingSession->addToChain(sess);
		videoReceivingSession->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
		//videoReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
		ctx->video.track->setMediaHandler(videoReceivingSession);

		ctx->video.track->onOpen([this,wCtx = std::weak_ptr(ctx),userID, h = std::move(packetizerRawHandler)]() {
			if(auto ctx = wCtx.lock())
			{
				std::scoped_lock lck{ _localVideo.mutex, ctx->video.mutex };
				ctx->video.packetizerListener = _localVideo.packetizer->output()->onDataChanged(std::move(h));
			}
			else
				qCWarning(LC_RTC_SERVICE) << "Cannot begin send video packets, peer" << userID << "isReleased";

		});
		ctx->video.track->onClosed([this, wCtx = std::weak_ptr(ctx),userID]() {
			qCDebug(LC_RTC_SERVICE) << "Video track width" << userID << "closed";
		});
		ctx->video.track->onFrame([this, userID,wCtx = std::weak_ptr(ctx)](rtc::binary data, rtc::FrameInfo info) {
			auto ctx = wCtx.lock();
			if (!ctx)
			{
				qCWarning(LC_RTC_SERVICE) << "Cannot handle video packet, peer" << userID << "isReleased";
				return;
			}
			std::lock_guard g{ctx->video.mutex };

			if(!ctx->video.packetPipe)
				ctx->video.packetPipe = createNullBufferPacketPipe();
			if (!ctx->video.decoder)
			{
				ctx->video.decoder.reset(new Video::H264Decoder());
				ctx->video.decoder->start(ctx->video.packetPipe);
			}
			auto pipeData = ctx->video.packetPipe->tryHoldForWriting();
			if (!pipeData.has_value())
			{
				qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
				return;
			}
			ctx->video.packets[pipeData->subpipe] = std::move(data);

			if (!Media::fillPacket(pipeData->ptr, (uint8_t*)ctx->video.packets[pipeData->subpipe].data(),
				ctx->video.packets[pipeData->subpipe].size())) 
			{
				qCWarning(LC_RTC_SERVICE) << "Cannot fill video packet";
				return;
			}
			pipeData->ptr->pts = info.timestamp;
			pipeData->ptr->dts = info.timestamp;
			ctx->video.packetPipe->unmapWriting(pipeData->subpipe, true);
			});
		pc->setLocalDescription();
	}else
		ctx->video.packetizerListener = _localVideo.packetizer->output()->onDataChanged(std::move(packetizerRawHandler));
}
Service::PeerContext::~PeerContext()
{
	//force close all tracks on context destruction 
	{
		std::lock_guard g(audio.mutex);
		if (audio.track)
			audio.track->close();
	}
	{
		std::lock_guard g(video.mutex);
		if (video.track)
			video.track->close();
	}

}
void Service::closeUserConnection(int userID)
{
	qCDebug(LC_RTC_SERVICE) << "Closing peer connection with" <<userID;
	flushRemoteAudio(userID);
	flushRemoteVideo(userID);;
	std::shared_ptr<Service::PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getPeerContext(userID);
	}
	if (!ctx)
		return;
	{
		std::scoped_lock lck{ _localVideo.mutex, ctx->video.mutex };
		//if user listen our video, then disable sending
		if (ctx->video.packetizerListener.has_value())
		{
			_localVideo.packetizer->output()->removeListener(ctx->video.packetizerListener.value());
			ctx->video.packetizerListener = std::nullopt;
		}
	}
	{
		std::scoped_lock lck{ _localAudio.mutex, ctx->audio.mutex };
		//if user listen our audio, then disable sending
		if (ctx->audio.encoderListener.has_value())
		{
			_localAudio.encoder->output()->removeListener(ctx->audio.encoderListener.value());
			ctx->audio.encoderListener = std::nullopt;
		}
	}
	//destroy context
	{
		std::lock_guard g(_peerMutex);
		_peerContexts.erase(userID);
	}
}
void Service::closeAllConnections()
{
	//closes all peer connections and reset codecs
	qCDebug(LC_RTC_SERVICE) << "Closing all connections";
	{
		std::scoped_lock lck{ _localVideo.mutex, _localAudio.mutex };
		if (_localVideo.encoder)
			_localVideo.encoder->close();
		if (_localVideo.packetizer)
			_localVideo.packetizer->stop();
		if (_localAudio.encoder)
			_localAudio.encoder->close();

		_localVideo.encoder.reset();
		_localVideo.packetizer.reset();
		_localAudio.encoder.reset();
	}
	{
		std::lock_guard g(_peerMutex);
		_peerContexts.clear();
	}
}
void Service::flushRemoteVideo(int userID)
{
	qCDebug(LC_RTC_SERVICE) << "Flush remote video from" << userID;
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getPeerContext(userID);;
	}
	if(ctx)
	{
		std::lock_guard g(ctx->video.mutex); 
		ctx->video.packetPipe.reset();
		ctx->video.decoder.reset();
	}
}
void Service::flushRemoteAudio(int userID)
{
	qCDebug(LC_RTC_SERVICE) << "Flush remote audio from" << userID;
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getPeerContext(userID);
	}
	if (ctx)
	{
		std::lock_guard g(ctx->audio.mutex);
		ctx->audio.packetPipe.reset();
		ctx->audio.decoder.reset();
	}
}
std::shared_ptr<Media::FramePipe> Service::getRemoteAudio(int userID)
{
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getOrCreatePeerContext(userID);
	}
	std::lock_guard ga(ctx->audio.mutex);
	if (!ctx->audio.packetPipe)
		ctx->audio.packetPipe = createNullBufferPacketPipe();
	if (!ctx->audio.decoder)
	{
		ctx->audio.decoder = std::make_shared<Media::Audio::OpusDecoder>();
		ctx->audio.decoder->start(ctx->audio.packetPipe);
	}
	return ctx->audio.decoder->output();
}
void Service::openLocalAudio(int userID, std::shared_ptr<Media::FramePipe> input, Media::Audio::SourceConfig config)
{
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getOrCreatePeerContext(userID);
	}
	std::scoped_lock lck{ _localAudio.mutex, ctx->audio.mutex };
	if(!_localAudio.encoder)
	{
		_localAudio.encoder = std::make_shared<Media::Audio::OpusEncoder>();
	}
	if (_localAudio.encoder->input() != input)
		_localAudio.encoder->start(input, std::move(config));

	auto encHandler = [this, userID,wCtx = std::weak_ptr(ctx)](std::shared_ptr<AVPacket> packet, size_t index) {
		auto ctx = wCtx.lock();
		if (!ctx)
		{
			qCWarning(LC_RTC_SERVICE) << "Cannot send audio packet, peer" << userID << "isReleased";
			_localAudio.encoder->output()->unmapReading(index);
			return;
		}
		//std::scoped_lock lck{_localAudio.mutex, ctx->audio.mutex };
		try
		{
			auto rtpConfig = ctx->audio.rtcp->rtpConfig;
			auto elapsedSeconds = time_since_epoch();
			// get elapsed time in clock rate
			uint32_t elapsedTimestamp = rtpConfig->secondsToTimestamp(elapsedSeconds);
			// set new timestamp
			rtpConfig->timestamp = rtpConfig->startTimestamp + elapsedTimestamp;

			// get elapsed time in clock rate from last RTCP sender report
			auto reportElapsedTimestamp = rtpConfig->timestamp - ctx->audio.rtcp->lastReportedTimestamp();
			// check if last report was at least 1 second ago
			if (rtpConfig->timestampToSeconds(reportElapsedTimestamp) > 1) {
				ctx->audio.rtcp->setNeedsToReport();
			}
			ctx->audio.track->send((std::byte*)packet->data, packet->size);
		}
		catch (std::exception& e)
		{
			qCWarning(LC_RTC_SERVICE) << "Cannot send packet to peer" << userID << ":" << e.what();
		}
		_localAudio.encoder->output()->unmapReading(index);
	};
	if(!ctx->audio.track)
	{
		rtc::Description::Audio media("audio", Description::Direction::SendRecv);
		media.addOpusCodec(111);
		media.addSSRC(2, "audio_stream");
		std::shared_ptr<PeerConnection> pc = ctx->pc;
		//_localAudio.encoder->start(input);
		ctx->audio.track = pc->addTrack(media);
		// create RTP configuration
		auto rtpConfig = make_shared<RtpPacketizationConfig>(2, "audio_stream", 111, OpusRtpPacketizer::DefaultClockRate);
		// create packetizer
		auto packetizer = make_shared<OpusRtpPacketizer>(rtpConfig);
		// add RTCP SR handler
		ctx->audio.rtcp = make_shared<RtcpSrReporter>(rtpConfig);
		packetizer->addToChain(ctx->audio.rtcp);
		// add RTCP NACK handler
/*		auto nack = make_shared<RtcpNackResponder>();
		packetizer->addToChain(nack)*/;

		auto audioReceivingSession = std::make_shared<rtc::OpusRtpDepacketizer>();
		auto sess = std::make_shared<RtcpReceivingSession>();
		audioReceivingSession->addToChain(sess);
		//audioReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
		ctx->audio.track->chainMediaHandler(audioReceivingSession);
		// set handler
		ctx->audio.track->chainMediaHandler(packetizer);
		ctx->audio.track->onOpen([this, wctx = std::weak_ptr(ctx), cb = std::move(encHandler),userID]() {
			if(auto ctx = wctx.lock())
			{
				std::scoped_lock lck{ _localAudio.mutex, ctx->audio.mutex };
				ctx->audio.encoderListener = _localAudio.encoder->output()->onDataChanged(std::move(cb));
			}
			else
				qCWarning(LC_RTC_SERVICE) << "Cannot begin send audio packets, peer" << userID << "isReleased";
		});
		ctx->audio.track->onClosed([userID]() {
			qCDebug(LC_RTC_SERVICE) << "Audio track width" << userID << "closed";
		});
		ctx->audio.track->onFrame([this, userID, wctx = std::weak_ptr(ctx)](rtc::binary data, rtc::FrameInfo info) {
			auto ctx = wctx.lock();
			if (!ctx)
				return;
			std::lock_guard g(ctx->audio.mutex);
			if (!ctx->audio.packetPipe)
				ctx->audio.packetPipe = createNullBufferPacketPipe();
			if (!ctx->audio.decoder)
			{
				ctx->audio.decoder.reset(new Audio::OpusDecoder());
				ctx->audio.decoder->start(ctx->audio.packetPipe);
			}
			auto pipeData = ctx->audio.packetPipe->tryHoldForWriting();
			if (!pipeData.has_value())
			{
				qCWarning(LC_RTC_SERVICE) << "Output pipe overflow";
			}
			ctx->audio.packets[pipeData->subpipe] = std::move(data);

			if (!Media::fillPacket(pipeData->ptr, (uint8_t*)ctx->audio.packets[pipeData->subpipe].data(),
				ctx->audio.packets[pipeData->subpipe].size()))
			{
				qCWarning(LC_RTC_SERVICE) << "Cannot fill audio packet";
				return;
			}
			pipeData->ptr->pts = info.timestamp;
			pipeData->ptr->dts = info.timestamp;
			ctx->audio.packetPipe->unmapWriting(pipeData->subpipe, true);
		});
		pc->setLocalDescription();
	}
	else
		ctx->audio.encoderListener = _localAudio.encoder->output()->onDataChanged(std::move(encHandler));
}
Service::Service(std::shared_ptr<NetworkCoordinator> coord, rtc::Configuration config)
	:_config(std::move(config))
    ,_coordinator(coord)
{
	_config.forceMediaTransport = true;
	rtc::InitLogger(rtc::LogLevel::Warning);
	Api::Description::handle(_coordinator, [this](Data::Description msg) {
		if (msg.id == Data::invalidID)
		{
			qCWarning(LC_RTC_SERVICE) << "Invalid userID in description received";
			return;
		}
		std::shared_ptr<PeerContext> ctx;
		{
			std::lock_guard g(_peerMutex);
			ctx = getOrCreatePeerContext(msg.id);
		}
		auto desc = rtc::Description(msg.description, msg.type);
		ctx->pc->setRemoteDescription(desc);
	});
	rtc::Api::Candidate::handle(_coordinator, [this](rtc::Data::Candidate candidate){
		std::lock_guard g(_peerMutex);
		std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(candidate.id);
		ctx->pc->addRemoteCandidate(rtc::Candidate(std::move(candidate.candidate),
			std::move(candidate.mid)));
	});
}
std::shared_ptr<Service::PeerContext> Service::getOrCreatePeerContext(int id)
{
	auto ctx = getPeerContext(id);
	if (!ctx)
	{
		createPeerContext(id);
		return getPeerContext(id);
	}
	return ctx;
}
void Service::createPeerContext(int id)
{
 	auto ctx = std::make_shared<PeerContext>();
	auto pc = std::make_shared<rtc::PeerConnection>(_config);
	ctx->pc = pc;
	_peerContexts.emplace(id, ctx);

	pc->onStateChange(
		[this,id, wctx = std::weak_ptr(ctx)](rtc::PeerConnection::State state) {
			std::cout << "State: " << state << std::endl; 
		});
	pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
		std::cout << "Gathering State: " << state << std::endl;
		});
	pc->onLocalDescription([this, id](rtc::Description description)
		{
			Api::Description outMsg;
			outMsg.id = id;
			outMsg.type = description.typeString();
			outMsg.description = std::move(description);
			outMsg.exec(_coordinator);
		});

	pc->onLocalCandidate([this, id](rtc::Candidate candidate)
		{
			Api::Candidate outMsg;
			outMsg.id = id;
			outMsg.mid = candidate.mid();
			outMsg.candidate = std::move(candidate);
			outMsg.exec(_coordinator);
		});
	pc->onTrack([this,id](std::shared_ptr<rtc::Track> track)
	{
		//remote peer opened video stream
		if (track->mid() == "video")
		{
			auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(1, "video-stream", 96, rtc::H264RtpPacketizer::defaultClockRate);
			auto videoReceivingSession = std::make_shared<rtc::H264RtpDepacketizer>(rtc::H264RtpDepacketizer::Separator::LongStartSequence);
			auto sess = std::make_shared<RtcpReceivingSession>();
			videoReceivingSession->addToChain(sess);
			videoReceivingSession->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
			//videoReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());

			track->setMediaHandler(videoReceivingSession);
			sess->requestKeyframe([](message_ptr message) {
				});

			std::shared_ptr<PeerContext> ctx = getPeerContext(id);
			ctx->video.track = track;
			if (!ctx->video.packetPipe)
				ctx->video.packetPipe = createNullBufferPacketPipe();

 			track->onClosed([this,id, wCtx = std::weak_ptr(ctx)]() {
				qCDebug(LC_RTC_SERVICE) << "Video track width" << id << "closed";
			});
			track->onFrame([this, id, wCtx = std::weak_ptr(ctx)](rtc::binary data, rtc::FrameInfo info) {
				auto ctx = wCtx.lock();
				if (!ctx)
				{
					qCWarning(LC_RTC_SERVICE) << "Cannot handle video packet, peer" << id << "isReleased";
					return;
				}
				std::lock_guard g(ctx->video.mutex);
				auto pipeData = ctx->video.packetPipe->tryHoldForWriting();
				if (!pipeData.has_value())
				{
					qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
					return;
				}
				ctx->video.packets[pipeData->subpipe] = std::move(data);

				if (!Media::fillPacket(pipeData->ptr, (uint8_t*)ctx->video.packets[pipeData->subpipe].data(),
					ctx->video.packets[pipeData->subpipe].size()))
				{
					qCWarning(LC_RTC_SERVICE) << "Cannot fill video packet";
					return;
				}
				pipeData->ptr->pts = info.timestamp;
				pipeData->ptr->dts = info.timestamp;
				ctx->video.packetPipe->unmapWriting(pipeData->subpipe, true);
			});
		}
		//remote peer opened audio stream
		if (track->mid() == "audio")
		{
			auto audioReceivingSession = std::make_shared<rtc::OpusRtpDepacketizer>();
			auto sess = std::make_shared<RtcpReceivingSession>();
			audioReceivingSession->addToChain(sess);
			//audioReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());

			// create RTP configuration
			auto rtpConfig = make_shared<RtpPacketizationConfig>(2, "audio", 111, OpusRtpPacketizer::DefaultClockRate);
			// create packetizer
			auto packetizer = make_shared<OpusRtpPacketizer>(rtpConfig);
			// add RTCP SR handler
			auto rtcp = make_shared<RtcpSrReporter>(rtpConfig);
			packetizer->addToChain(rtcp);
			// add RTCP NACK handler
			//auto nack = make_shared<RtcpNackResponder>();
			//packetizer->addToChain(nack);
			track->chainMediaHandler(audioReceivingSession);
			track->chainMediaHandler(packetizer);
			sess->requestKeyframe([](message_ptr message) {
				});
			std::shared_ptr<PeerContext> ctx = getPeerContext(id);
			ctx->audio.rtcp = rtcp;
			ctx->audio.track = track;
			if (!ctx->audio.decoder)
				ctx->audio.decoder.reset(new Audio::OpusDecoder());
			if(!ctx->audio.packetPipe)
			{
				ctx->audio.packetPipe = createNullBufferPacketPipe();
				ctx->audio.decoder->start(ctx->audio.packetPipe);
			}
			track->onClosed([id]() {
				qCDebug(LC_RTC_SERVICE) << "Audio track width" << id << "closed";
			});
			track->onFrame([wCtx = std::weak_ptr(ctx),id](rtc::binary data, rtc::FrameInfo info) {
				if (auto ctx = wCtx.lock())
				{
					std::lock_guard g(ctx->audio.mutex);
					auto pipeData = ctx->audio.packetPipe->tryHoldForWriting();
					if (!pipeData.has_value())
					{
						qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
						return;
					}
					ctx->audio.packets[pipeData->subpipe] = std::move(data);

					if (!Media::fillPacket(pipeData->ptr, (uint8_t*)ctx->audio.packets[pipeData->subpipe].data(),
						ctx->audio.packets[pipeData->subpipe].size()))
					{
						qCWarning(LC_RTC_SERVICE) << "Cannot fill audio packet";
						return;
					}
					pipeData->ptr->pts = info.timestamp;
					pipeData->ptr->dts = info.timestamp;
					ctx->audio.packetPipe->unmapWriting(pipeData->subpipe, true);
				}else
					qCWarning(LC_RTC_SERVICE) << "Cannot handle audio packet, peer" << id << "isReleased";

			});
		}
	});
}
std::shared_ptr<Media::FramePipe> Service::getRemoteVideo(int userID)
{
	std::shared_ptr<PeerContext> ctx;
	{
		std::lock_guard g(_peerMutex);
		ctx = getOrCreatePeerContext(userID);
	}
	std::lock_guard gv(ctx->video.mutex);
	if(!ctx->video.packetPipe)
		ctx->video.packetPipe = createNullBufferPacketPipe();
	if (!ctx->video.decoder)
	{
		ctx->video.decoder.reset(new Video::H264Decoder());
		ctx->video.decoder->start(ctx->video.packetPipe);
	}
	return ctx->video.decoder->output();
}
std::shared_ptr<Service::PeerContext> Service::getPeerContext(int id)
{
	if (_peerContexts.contains(id))
		return _peerContexts[id];
	return nullptr;
}
QDebug operator<<(QDebug debug, const rtc::PeerConnection::GatheringState& s)
{
	using GatheringState = PeerConnection::GatheringState;
	const char* str;
	switch (s) {
	case GatheringState::New:
		str = "new";
		break;
	case GatheringState::InProgress:
		str = "in-progress";
		break;
	case GatheringState::Complete:
		str = "complete";
		break;
	default:
		str = "unknown";
		break;
	}
	return debug << str;
}
QDebug operator<<(QDebug debug, const rtc::PeerConnection::State& s)
{
	using State = PeerConnection::State;
	const char* str;
	switch (s) {
	case State::New:
		str = "new";
		break;
	case State::Connecting:
		str = "connecting";
		break;
	case State::Connected:
		str = "connected";
		break;
	case State::Disconnected:
		str = "disconnected";
		break;
	case State::Failed:
		str = "failed";
		break;
	case State::Closed:
		str = "closed";
		break;
	default:
		str = "unknown";
		break;
	}
	return debug << str;
}
void Service::closeLocalAudio(int userID)
{
	std::lock_guard g(_peerMutex);
	for (auto& ctx : _peerContexts)
	{
		std::scoped_lock lck{ _localAudio.mutex, ctx.second->audio.mutex };
		if (ctx.second->audio.encoderListener.has_value())
			_localAudio.encoder->output()->removeListener(ctx.second->audio.encoderListener.value());
	}
}
void Service::closeLocalVideo(int userID)
{
	std::lock_guard g(_peerMutex);
	for (auto& ctx : _peerContexts)
	{
		std::scoped_lock lck{ _localVideo.mutex, ctx.second->video.mutex };
		if (ctx.second->video.packetizerListener.has_value())
			_localVideo.packetizer->output()->removeListener(ctx.second->video.packetizerListener.value());
	}
}