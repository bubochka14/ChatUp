#pragma once

#include <WinSock2.h>
#include <QFuture>
#include <rtc/rtc.hpp>
#include <qloggingcategory.h>
#include "network_include.h"
#include "network.h"
#include "networkcoordinator.h"
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

	class CC_NETWORK_EXPORT Service
	{
	public:
		explicit Service(std::shared_ptr<NetworkCoordinator>, rtc::Configuration config);
		void openLocalVideo(int userID,std::shared_ptr<Media::FramePipe> input, Media::Video::SourceConfig config);
		void openLocalAudio(int userID, std::shared_ptr<Media::FramePipe> input, Media::Audio::SourceConfig config);
		std::shared_ptr<Media::FramePipe> getRemoteVideo(int userID);
		std::shared_ptr<Media::FramePipe> getRemoteAudio(int userID);
		void closeLocalVideo(int userID);
		void closeLocalAudio(int userID);
		void flushRemoteVideo(int userID);
		void flushRemoteAudio(int userID);
		void closeUserConnection(int userID);

		//closes all peer connections and reset codecs
		void closeAllConnections();
	protected:
		struct RemoteAudioContext
		{
			std::shared_ptr<rtc::Track> track;
			std::shared_ptr<Media::AbstractDecoder> decoder;
			std::array<std::vector<std::byte>, Media::PacketPipe::getSize()> packets;
			std::shared_ptr<Media::PacketPipe> packetPipe;
			std::shared_ptr<rtc::RtcpSrReporter> rtcp;
			///std::shared_ptr<rtc::RtpPacketizationConfig> audioConfig;
			std::optional<int> encoderListener;
			std::mutex mutex;
		};
		struct RemoteVideoContext
		{
			std::shared_ptr<rtc::Track> track;
			std::array<std::vector<std::byte>, Media::PacketPipe::getSize()> packets;
			std::shared_ptr<Media::AbstractDecoder> decoder;
			std::shared_ptr<Media::PacketPipe> packetPipe;
			std::optional<int> packetizerListener;
			std::mutex mutex;

		};
		struct LocalVideoContext
		{
			std::shared_ptr<Media::Video::AbstractEncoder> encoder;
			std::shared_ptr<Media::RtpPacketizer> packetizer;
			std::mutex mutex;

		};
		struct LocalAudioContext
		{
			std::shared_ptr<Media::Audio::AbstractEncoder> encoder;
			std::mutex mutex;

		};
		struct PeerContext
		{
			RemoteAudioContext audio;
			RemoteVideoContext video;
			std::shared_ptr<rtc::PeerConnection> pc;

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
		LocalAudioContext _localAudio;
		LocalVideoContext _localVideo;
		std::optional<int> _packetizerListener;

	};
}
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::GatheringState& s);
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::State& s);