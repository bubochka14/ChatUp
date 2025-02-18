#include "rtcservice.h"
using namespace rtc;
using namespace std;
using namespace Media;
Q_LOGGING_CATEGORY(LC_RTC_SERVICE, "RTCService");
PeerConnectionHandle::PeerConnectionHandle(std::shared_ptr<PeerConnection> pc, int userID)
	:pc(pc)
	,_userID(userID)
{
	pc->onTrack([this](std::shared_ptr<rtc::Track> track)
		{

			std::cout << "Track open, direction: " << track->direction()
				<< "\ndescription: " << std::string(track->description());

			if (track->mid() == "video")
			{

				auto rtpConfig = std::make_shared<rtc::RtpPacketizationConfig>(42, "channelName", 96, rtc::H264RtpPacketizer::defaultClockRate);
				auto videoReceivingSession = std::make_shared<rtc::H264RtpDepacketizer>(rtc::H264RtpDepacketizer::Separator::LongStartSequence);
				auto sess = std::make_shared<RtcpReceivingSession>();
				videoReceivingSession->addToChain(sess);
				AVCodecParameters* pars = nullptr;
				videoReceivingSession->addToChain(std::make_shared<rtc::RtcpSrReporter>(rtpConfig));
				videoReceivingSession->addToChain(std::make_shared<rtc::RtcpNackResponder>());
				track->setMediaHandler(videoReceivingSession);

				sess->requestKeyframe([](message_ptr message) {
					});
				videoTrack = track;
			/*	if (!_videoCb.has_value())
					return;*/
				_videoDecoder.reset(new Video::H264Decoder());
				_videoRaw = std::make_shared<Media::RawPipe>();
				_videoDemuxer = std::make_shared<Media::Video::H264Demuxer>();
				_videoDecoder->start(_videoDemuxer->output());
				_videoDemuxer->start(_videoRaw);
				if (_videoCb.has_value())
					_videoCb.value()(_videoDecoder->output());
			}
			track->onMessage([](rtc::binary data)
				{
					qDebug() << "received message" << data.size();
				},
				[](std::string error)
				{
					qDebug() << "received error" << error;

				});
			track->onFrame([this](rtc::binary data, rtc::FrameInfo info) {
				auto pipeData = _videoRaw->holdForWriting();
				pipeData.ptr->raw = (uint8_t*)data.data();
				pipeData.ptr->size = data.size();
				_videoRaw->unmapWriting(pipeData.subpipe,true);
				});
		});

}
int PeerConnectionHandle::userID()
{
	return _userID;
}
QFuture<void> PeerConnectionHandle::openLocalVideo(std::shared_ptr<FramePipe> input,Video::SourceConfig conf)
{
		if (!_videoEncoder)
			_videoEncoder = std::make_shared<Media::Video::H264Encoder>();
		rtc::Description::Video media("video", rtc::Description::Direction::SendRecv);
		media.addH264Codec(96); // Must match the payload type of the external h264 RTP stream
		media.addSSRC(42, "video-send");
		videoTrack = pc->addTrack(media);
		auto rtpConfig = make_shared<RtpPacketizationConfig>(42, "video-send", 96, H264RtpPacketizer::defaultClockRate);
		// create packetizer
		auto packetizer = make_shared<H264RtpPacketizer>(NalUnit::Separator::LongStartSequence, rtpConfig);
		// add RTCP SR handler
		auto reporter = make_shared<RtcpSrReporter>(rtpConfig);
		packetizer->addToChain(reporter);
		// add RTCP NACK handler
		auto nackResponder = make_shared<RtcpNackResponder>();
		packetizer->addToChain(nackResponder);
		_videoEncoder->start(input, std::move(conf));
		PacketizationConfig pConfig;
		pConfig.ecnCtx = _videoEncoder->codecContext();
		_videoPacketizer = std::make_shared<Media::RtpPacketizer>(pConfig);
		_videoPacketizer->start(_videoEncoder->output());
		videoTrack->onOpen([wInput = std::weak_ptr(input), this, conf]() {
			auto input = wInput.lock();
			if (!input)
				return;
			_videoPacketizer->output()->onDataChanged([this](std::weak_ptr<Media::Raw> wraw, size_t index) {
					auto raw = wraw.lock();
					if (!raw)
						return;
					videoTrack->send((std::byte*)raw->raw, raw->size);
					_videoPacketizer->output()->unmapReading(index);
				});

		});
		pc->setLocalDescription();

	return QtFuture::makeReadyVoidFuture();
}
void PeerConnectionHandle::onRemoteVideoOpen(VideoCallBack cb)
{
	_videoCb = std::move(cb);
}
//
//bool PeerConnectionHandle::openAudio(std::shared_ptr<Media::PacketPipe> input)
//{
//	return true;
//}

Service::Service(std::shared_ptr<NetworkCoordinator> coord, rtc::Configuration config)
	:_config(std::move(config))
    ,_coordinator(std::move(coord))
{
	rtc::InitLogger(rtc::LogLevel::Warning);
	Api::Description::handle(_coordinator, [this](Data::Description msg)
		{
			if (msg.id == Data::invalidID)
			{
				qCWarning(LC_RTC_SERVICE) << "Invalid userID in description received";
				return;
			}
			std::shared_ptr<rtc::PeerConnectionHandle> peerHandle = nullptr;
			if (auto jt = _peerHandles.find(msg.id); jt != _peerHandles.end()) {
				peerHandle = jt->second;
			}
			else if (msg.type == "offer") {
				std::cout << "Answering to " + msg.id << std::endl;
				peerHandle = createPeerHandle(msg.id);
			}
			else {
				return;
			}
			peerHandle->pc->setRemoteDescription(rtc::Description(msg.description, msg.type));

		});
	rtc::Api::Candidate::handle(_coordinator, [this](rtc::Data::Candidate candidate)
		{
			if (auto jt = _peerHandles.find(candidate.id); jt != _peerHandles.end()) {
				jt->second->pc->addRemoteCandidate(rtc::Candidate(std::move(candidate.candidate),
					std::move(candidate.mid)));
			}
		});
}
std::shared_ptr<rtc::PeerConnectionHandle> Service::createPeerHandle(int id)
{
	auto pc = std::make_shared<rtc::PeerConnection>(_config);

	pc->onStateChange(
		[](rtc::PeerConnection::State state) { std::cout << "State: " << state << std::endl; });

	pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state) {
		std::cout << "Gathering State: " << state << std::endl;
		});
	pc->onLocalDescription([this, id](rtc::Description description) 
		{
			qDebug() << "LOCAL DESACRIPTION";
			Api::Description outMsg;
			outMsg.id = id;
			outMsg.type = description.typeString();
			outMsg.description = std::move(description);
			outMsg.exec(_coordinator);
		});

	pc->onLocalCandidate([this, id](rtc::Candidate candidate) 
		{
			qDebug() << "LOCAL Candidate";

			Api::Candidate outMsg;
			outMsg.id = id;
			outMsg.mid = candidate.mid();
			outMsg.candidate = std::move(candidate);
			outMsg.exec(_coordinator);
		});
	auto pcHandle = std::make_shared<PeerConnectionHandle>(pc,id);
	_peerHandles.emplace(id, pcHandle);
	return pcHandle;
}
std::shared_ptr<PeerConnectionHandle> Service::getPeerConnectionHandle(int userID)
{
	if (!_peerHandles.contains(userID))
		return createPeerHandle(userID);
	return _peerHandles[userID];
}
QFuture<void> Service::establishPeerConnection(int userID)
{
	if (_peerHandles.contains(userID))
		return QtFuture::makeExceptionalFuture(make_exception_ptr("Peer already exist"));
	qCDebug(LC_RTC_SERVICE) << "Establishing peer connection with user" << userID;
 	auto pcHandle = createPeerHandle(userID);
	//rtc::Description::Video media("video", rtc::Description::Direction::RecvOnly);
	//media.addH264Codec(96); // Must match the payload type of the external h264 RTP stream
	//media.addSSRC(42, "video-send");
	//videoTrack = pc->addTrack(media);
	//pc->setLocalDescription();
	return QtFuture::makeReadyVoidFuture();
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