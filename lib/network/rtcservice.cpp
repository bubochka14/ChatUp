#include "rtcservice.h"
using namespace rtc;
using namespace std;
using namespace Media;
Q_LOGGING_CATEGORY(LC_RTC_SERVICE, "RTCService");
QtEventLoopEmplacer* ev;
//PeerConnectionHandle::PeerConnectionHandle(std::shared_ptr<PeerConnection> pc, int userID)
//	:pc(pc)
//	,_userID(userID)
//{
//	
//
//}

std::chrono::system_clock::rep time_since_epoch() {
	static_assert(
		std::is_integral<std::chrono::system_clock::rep>::value,
		"Representation of ticks isn't an integral value."
		);
	auto now = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(now).count();
}
QFuture<void> Service::openLocalVideo(int userID,std::shared_ptr<FramePipe> input)
{
	std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(userID);
	std::lock_guard g(ctx->mutex);
	_videoEncoder = std::make_shared<Media::Video::H264Encoder>();
	_videoEncoder->start(input);
	PacketizationConfig pConfig;
	pConfig.ecnCtx = _videoEncoder->codecContext();
	_videoPacketizer = std::make_shared<Media::RtpPacketizer>(pConfig);
	_videoPacketizer->start(_videoEncoder->output());
	auto packetizerRawHandler = [this,userID](std::shared_ptr<Media::Raw> raw, size_t index) {
		auto ctx = getPeerContext(userID);
		if (!ctx)
			return;
		try {
			ctx->videoTrack->send((std::byte*)raw->raw, raw->size);
		}
		catch (std::exception& e)
		{
			qCWarning(LC_RTC_SERVICE) << "Cannot send video packet" << e.what();
		}
		_videoPacketizer->output()->unmapReading(index);
		};
	if (!ctx->videoTrack)
	{
		rtc::Description::Video media("video", rtc::Description::Direction::SendRecv);
		media.addH264Codec(96); // Must match the payload type of the external h264 RTP stream
		media.addSSRC(userID, "video-stream");
		std::shared_ptr<PeerConnection> pc = ctx->pc;
		ctx->videoTrack = pc->addTrack(media);
		auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(42, "video-stream", 96, rtc::H264RtpPacketizer::defaultClockRate);
		auto videoReceivingSession = std::make_shared<rtc::H264RtpDepacketizer>(rtc::H264RtpDepacketizer::Separator::LongStartSequence);
		auto sess = std::make_shared<RtcpReceivingSession>();
		videoReceivingSession->addToChain(sess);
		videoReceivingSession->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
		videoReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
		ctx->videoTrack->setMediaHandler(videoReceivingSession);

		ctx->videoTrack->onOpen([this,h = std::move(packetizerRawHandler)]() {
			_videoPacketizer->output()->onDataChanged(std::move(h));
			});
		ctx->videoTrack->onFrame([this, userID](rtc::binary data, rtc::FrameInfo info) {
			std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(userID);
			if(!ctx->videoPackets)
				ctx->videoPackets = Media::createPacketPipe();
			if (!ctx->videoDecoder)
			{
				ctx->videoDecoder.reset(new Video::H264Decoder());
				ctx->videoDecoder->start(ctx->videoPackets);
			}
			auto pipeData = ctx->videoPackets->tryHoldForWriting();
			if (!pipeData.has_value())
			{
				qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
				return;
			}
			Media::fillPacket(pipeData->ptr, (uint8_t*)data.data(), data.size());
			pipeData->ptr->pts = info.timestamp;
			pipeData->ptr->dts = info.timestamp;
			ctx->videoPackets->unmapWriting(pipeData->subpipe, true);
			});
		pc->setLocalDescription();
	}else
		_videoPacketizer->output()->onDataChanged(std::move(packetizerRawHandler));

	return QtFuture::makeReadyVoidFuture();
}
void Service::flushInput(int userID)
{
	qCDebug(LC_RTC_SERVICE) << "Flush input from" << userID;
	std::shared_ptr<PeerContext> ctx = getPeerContext(userID);
	if(ctx)
	{
		std::lock_guard g(ctx->mutex);
		ctx->videoPackets.reset();
		ctx->videoDecoder.reset();
	}
}

std::optional<rtc::Track> Service::openLocalAudio(int userID, std::shared_ptr<Media::FramePipe> input)
{
	std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(userID);
	if (!_audioEncoder)
		_audioEncoder = std::make_shared<Media::Audio::OpusEncoder>();
	rtc::Description::Audio media("audio");
	media.addOpusCodec(111);
	media.addSSRC(userID, "audio","audio_stream","audio");
	std::shared_ptr<PeerConnection> pc = ctx->pc;
	//_audioEncoder->start(input);
	ctx->audioTrack = pc->addTrack(media);
	_audioEncoder->start(input);
	// create RTP configuration
	auto rtpConfig = make_shared<RtpPacketizationConfig>(userID, "audio", 111, OpusRtpPacketizer::DefaultClockRate);
	// create packetizer
	auto packetizer = make_shared<OpusRtpPacketizer>(rtpConfig);
	// add RTCP SR handler
	ctx->rtcp = make_shared<RtcpSrReporter>(rtpConfig);
	packetizer->addToChain(ctx->rtcp);
	// add RTCP NACK handler
	auto nack = make_shared<RtcpNackResponder>();
	packetizer->addToChain(nack);
	// set handler
	ctx->audioTrack->setMediaHandler(packetizer);
	ctx->audioTrack->onOpen([this, wInput = std::weak_ptr(input), wCtx = std::weak_ptr(ctx)]() {
		auto input = wInput.lock();
		if (!input)
			return;
		_audioEncoder->output()->onDataChanged([this, wCtx](std::shared_ptr<AVPacket> packet, size_t index) {
			if (auto ctx = wCtx.lock())
			{
				auto elapsedSeconds = time_since_epoch();
				auto rtpConfig = ctx->rtcp->rtpConfig;
				// get elapsed time in clock rate
				uint32_t elapsedTimestamp = rtpConfig->secondsToTimestamp(elapsedSeconds);
				// set new timestamp
				rtpConfig->timestamp = rtpConfig->startTimestamp + elapsedTimestamp;

				// get elapsed time in clock rate from last RTCP sender report
				auto reportElapsedTimestamp = rtpConfig->timestamp - ctx->rtcp->lastReportedTimestamp();
				// check if last report was at least 1 second ago
				if (rtpConfig->timestampToSeconds(reportElapsedTimestamp) > 1) {
					ctx->rtcp->setNeedsToReport();
				}
				ctx->audioTrack->send((std::byte*)packet->data, packet->size);
				_audioEncoder->output()->unmapReading(index);
			}
		});
	});
	pc->setLocalDescription();
	return nullopt;
}
Service::Service(std::shared_ptr<NetworkCoordinator> coord, rtc::Configuration config)
	:_config(std::move(config))
    ,_coordinator(coord)
{
	_config.forceMediaTransport = true;
	rtc::InitLogger(rtc::LogLevel::Warning);
	ev = new QtEventLoopEmplacer;
	Api::Description::handle(_coordinator, [this](Data::Description msg) {
		if (msg.id == Data::invalidID)
		{
			qCWarning(LC_RTC_SERVICE) << "Invalid userID in description received";
			return;
		}
		std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(msg.id);
		std::lock_guard g(ctx->mutex);
		auto desc = rtc::Description(msg.description, msg.type);
		ctx->pc->setRemoteDescription(desc);
		//for (size_t i = 0; i < desc.mediaCount(); i++)
		//{
		//	auto var = desc.media(i);
		//	if (!std::holds_alternative<Description::Media*>(var))
		//		continue;
		//	auto media = std::get<Description::Media*>(var);
		//	if (media->mid() == "video" && ctx->videoDecoder)
		//	{
		//		if (media->direction() == rtc::Description::Direction::Inactive ||
		//			media->direction() == rtc::Description::Direction::RecvOnly)
		//		{
		//			if (_closeVideoCb.has_value())
		//				_closeVideoCb.value()(msg.id);
		//			ctx->videoDecoder.reset();
		//			ctx->videoPackets.reset();
		//		}

		//	}
		//}

	});
	rtc::Api::Candidate::handle(_coordinator, [this](rtc::Data::Candidate candidate){
		std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(candidate.id);
		std::lock_guard g(ctx->mutex);
		ctx->pc->addRemoteCandidate(rtc::Candidate(std::move(candidate.candidate),
			std::move(candidate.mid)));
	});
}
std::shared_ptr<Service::PeerContext> Service::getOrCreatePeerContext(int id)
{
	std::lock_guard g(_peerMutex);
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
	std::lock_guard g(ctx->mutex);

	auto pc = std::make_shared<rtc::PeerConnection>(_config);
	ctx->pc = pc;
	_peerContexts.emplace(id, ctx);
	pc->onStateChange(
		[](rtc::PeerConnection::State state) { std::cout << "State: " << state << std::endl; });

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
		if (track->mid() == "video")
		{
		
			auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(42, "video-stream", 96, rtc::H264RtpPacketizer::defaultClockRate);
			auto videoReceivingSession = std::make_shared<rtc::H264RtpDepacketizer>(rtc::H264RtpDepacketizer::Separator::LongStartSequence);
			auto sess = std::make_shared<RtcpReceivingSession>();
			videoReceivingSession->addToChain(sess);
			videoReceivingSession->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
			videoReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
			track->setMediaHandler(videoReceivingSession);
			sess->requestKeyframe([](message_ptr message) {
				});
			std::shared_ptr<PeerContext> ctx = getPeerContext(id);
			ctx->videoTrack = track;
			if (!ctx->videoPackets)
				ctx->videoPackets = Media::createPacketPipe();
			track->onFrame([this, id](rtc::binary data, rtc::FrameInfo info) {
				std::shared_ptr<PeerContext> ctx = getPeerContext(id);
				auto pipeData = ctx->videoPackets->tryHoldForWriting();
				if (!pipeData.has_value())
				{
					qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
					return;
				}
				Media::fillPacket(pipeData->ptr, (uint8_t*)data.data(), data.size());
	/*			pipeData->ptr->pts = info.timestamp;
				pipeData->ptr->dts = info.timestamp;*/
				ctx->videoPackets->unmapWriting(pipeData->subpipe, true);
				});
		}
		if (track->mid() == "audio")
		{
			std::shared_ptr<PeerContext> ctx = getPeerContext(id);

			ctx->audioTrack = track;
			auto audioReceivingSession = std::make_shared<rtc::OpusRtpDepacketizer>();
			auto sess = std::make_shared<RtcpReceivingSession>();
			audioReceivingSession->addToChain(sess);
			audioReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
			track->setMediaHandler(audioReceivingSession);
			sess->requestKeyframe([](message_ptr message) {
				});
			ctx->audioDecoder.reset(new Audio::OpusDecoder());
			ctx->audioPackets = Media::createPacketPipe();
			ctx->audioDecoder->start(ctx->audioPackets);
			track->onFrame([this, id, wCtx = std::weak_ptr(ctx)](rtc::binary data, rtc::FrameInfo info) {
				if (auto ctx = wCtx.lock())
				{
					auto pipeData = ctx->audioPackets->tryHoldForWriting();
					if (!pipeData.has_value())
					{
						qCWarning(LC_RTC_SERVICE) << "Pipe overflow";
						return;
					}
					Media::fillPacket(pipeData->ptr, (uint8_t*)data.data(), data.size());
					pipeData->ptr->pts = info.timestamp;
					pipeData->ptr->dts = info.timestamp;
					ctx->audioPackets->unmapWriting(pipeData->subpipe, true);
				}
				});
			track->onMessage([this, id](rtc::message_variant data) {
				/*std::shared_ptr<PeerContext> ctx = getPeerContext(id);
				auto pipeData = ctx->videoRaw->holdForWriting();
				pipeData.ptr->raw = (uint8_t*)data.data();
				pipeData.ptr->size = data.size();
				ctx->videoRaw->unmapWriting(pipeData.subpipe, true);*/
				});
			if (_audioCb.has_value())
			{
				_audioCb.value()(id, ctx->audioDecoder->output());
			}
		}
	});
}
std::shared_ptr<Media::FramePipe> Service::getRemoteVideo(int userID)
{
	std::shared_ptr<PeerContext> ctx = getOrCreatePeerContext(userID);
	std::lock_guard g(ctx->mutex);
	if(!ctx->videoPackets)
		ctx->videoPackets = Media::createPacketPipe();
	if (!ctx->videoDecoder)
	{
		ctx->videoDecoder.reset(new Video::H264Decoder());
		ctx->videoDecoder->start(ctx->videoPackets);
	}
	return ctx->videoDecoder->output();
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
void Service::closeLocalVideo(int userID)
{
	_videoPacketizer.reset();
	_videoEncoder.reset();
	//if (!_peerContexts.contains(userID))
	//	return;
	//auto& context = _peerContexts[userID];
	//if (context->videoTrack->direction() == rtc::Description::Direction::SendRecv)
	//{
	//	_videoEncoder.reset();
	//	_videoPacketizer.reset();
	//	rtc::Description::Media dsc = context->videoTrack->description();
	//	dsc.setDirection(Description::Direction::RecvOnly);
	//	context->videoTrack = context->pc->addTrack(std::move(dsc));
	//	context->pc->setLocalDescription();
	//}
	//if (context->videoTrack->direction() == rtc::Description::Direction::SendOnly)
	//{
	//	rtc::Description::Media dsc = context->videoTrack->description();
	//	dsc.setDirection(Description::Direction::Inactive);
	//	context->videoTrack = context->pc->addTrack(std::move(dsc));
	//	context->pc->setLocalDescription();

	//}
}