#pragma once

#include <WinSock2.h>
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
#include <random>

#include "decoder.h"
#include "scopeguard.h"
Q_DECLARE_LOGGING_CATEGORY(LC_RTC_SERVICE);
namespace rtc
{
	struct LocalVideoDescription
	{
		enum VideoCodec
		{
			H264,
			H265
		};
		std::shared_ptr<Media::FramePipe> input;
		Media::Video::SourceConfig config;
		VideoCodec codec;
	};
	class CC_NETWORK_EXPORT Service
	{
	public:
		explicit Service(std::shared_ptr<NetworkCoordinator>, rtc::Configuration config);
		QFuture<void> openLocalVideo(int userID,std::shared_ptr<Media::FramePipe> input, Media::Video::SourceConfig config);
		QFuture<void> openLocalAudio(int userID, std::shared_ptr<Media::FramePipe> input, Media::Audio::SourceConfig config);
		std::shared_ptr<Media::FramePipe> getRemoteVideo(int userID);
		std::shared_ptr<Media::FramePipe> getRemoteAudio(int userID);
		void closeLocalVideo(int userID);
		void closeLocalAudio(int userID);
		void flushRemoteVideo(int userID);
		void flushRemoteAudio(int userID);
		void closeAllConnections();
		//void onPeerConnection(std::function<void(std::shared_ptr<PeerConnectionHandle>)> cb);
	protected:
		struct PeerContext
		{
			std::shared_ptr<rtc::Track> audioTrack;
			std::shared_ptr<rtc::Track> videoTrack;
			std::shared_ptr<Media::AbstractDecoder> videoDecoder;
			std::shared_ptr<Media::AbstractDecoder> audioDecoder;
			std::shared_ptr<Media::PacketPipe> audioPackets;
			std::shared_ptr<Media::PacketPipe> videoPackets;
			std::shared_ptr<rtc::PeerConnection> pc;
			std::shared_ptr<rtc::RtcpSrReporter> rtcp;
			std::shared_ptr<rtc::RtpPacketizationConfig> audioConfig;
			std::optional<int> videoPacketizerListener;
			std::optional<int> audioEncoderListener;
			std::mutex mutex;
			~PeerContext();
		};
		void createPeerContext(int id);

		std::shared_ptr<PeerContext> getPeerContext(int id);
		std::shared_ptr<PeerContext> getOrCreatePeerContext(int id);
	private:
		std::unordered_map<int, std::shared_ptr<PeerContext>> _peerContexts;
		std::mutex _peerMutex;
		rtc::Configuration _config;
		std::shared_ptr<NetworkCoordinator> _coordinator;
		std::optional<std::function<void(int)>> _closeVideoCb;
		std::shared_ptr<Media::Video::AbstractEncoder> _videoEncoder;
		std::shared_ptr<Media::Audio::AbstractEncoder> _audioEncoder;
		std::shared_ptr<Media::RtpPacketizer> _videoPacketizer;
		std::optional<int> _packetizerListener;
		bool _isClosingConnections = false;
		//std::shared_ptr<Media::RtpPacketizer> _audioPacketizer;


	};
}
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::GatheringState& s);
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::State& s);