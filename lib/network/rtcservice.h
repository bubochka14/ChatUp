#pragma once
#include <QFuture>
#include <rtc/rtc.hpp>
#include <qloggingcategory.h>
#include "network_include.h"
#include "network.h"
#include "networkmanager.h"
#include "api/sdp.h"
#include "encoder.h"
#include "media.h"
#include "rtppacketizer.h"
#include "decoder.h"
Q_DECLARE_LOGGING_CATEGORY(LC_RTC_SERVICE);
namespace rtc
{
	class Service;
	class PacketizationPipeline
	{
		void open(std::shared_ptr<Media::FramePipe> framePipe);

	};
	struct CC_NETWORK_EXPORT PeerConnectionHandle
	{
		using VideoCallBack = std::function<void(std::shared_ptr<Media::FramePipe>)>;

		PeerConnectionHandle(std::shared_ptr<PeerConnection> pc, int userID);
		QFuture<void> openLocalVideo(std::shared_ptr<Media::FramePipe> input, Media::Video::SourceConfig conf);
		QFuture<bool> openListenning();
		std::optional<rtc::Track> openLocalAudio(std::shared_ptr<Media::PacketPipe> input);
		void onRemoteVideo(std::function<void(VideoCallBack)>);
		void onRemoteVideoOpen(VideoCallBack cb);
		int userID();
	protected:
		std::shared_ptr<PeerConnection> pc;
		std::shared_ptr<rtc::Track> audioTrack;
		std::shared_ptr<rtc::Track> videoTrack;
		std::optional<VideoCallBack> _videoCb;
		std::shared_ptr<Media::Video::H264Encoder> _videoEncoder;
		std::shared_ptr<Media::RtpPacketizer> _videoPacketizer;
		std::shared_ptr<Media::AbstractDecoder> _videoDecoder;
		std::shared_ptr<Media::RawPipe> _videoRaw;
		std::shared_ptr<Media::Video::H264Demuxer> _videoDemuxer;
		int _userID;
		friend class Service;

	};
	class CC_NETWORK_EXPORT Service
	{
	public:
		explicit Service(std::shared_ptr<NetworkCoordinator>, rtc::Configuration config);
		QFuture<void> establishPeerConnection(int userID);
		std::shared_ptr<PeerConnectionHandle> getPeerConnectionHandle(int userID);
		void onPeerConnection(std::function<void(std::shared_ptr<PeerConnectionHandle>)> cb);
	private:
		std::shared_ptr<rtc::PeerConnectionHandle> createPeerHandle(int id);
		std::function<void(std::shared_ptr<PeerConnectionHandle>)> _peerCb;
		std::unordered_map<int, std::shared_ptr<PeerConnectionHandle>> _peerHandles;
		std::unordered_map<int, std::shared_ptr<QPromise<void>>> _pending;
		rtc::Configuration _config;
		std::shared_ptr<NetworkCoordinator> _coordinator;

	};
}
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::GatheringState& s);
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::State& s);