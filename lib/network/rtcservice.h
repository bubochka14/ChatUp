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
	//struct CC_NETWORK_EXPORT PeerConnectionHandle
	//{
	//	PeerConnectionHandle(std::shared_ptr<PeerConnection> pc, int userID);
	//	QFuture<void> openLocalVideo(std::shared_ptr<Media::FramePipe> input, Media::Video::SourceConfig conf);
	//	std::optional<rtc::Track> openLocalAudio(std::shared_ptr<Media::PacketPipe> input);
	//	void onRemoteVideo(std::function<void(VideoCallBack)>);
	//	void onRemoteVideoClosed(std::function<void()> cb);
	//	void onRemoteVideoOpen(VideoCallBack cb);
	//	int userID();
	//protected:
	//	std::shared_ptr<PeerConnection> pc;
	//	std::shared_ptr<rtc::Track> audioTrack;
	//	std::shared_ptr<rtc::Track> videoTrack;
	//	std::optional<VideoCallBack> _videoCb;
	//	std::optional<void()> _closeVideoCb;
	//	std::shared_ptr<Media::Video::H264Encoder> _videoEncoder;
	//	std::shared_ptr<Media::RtpPacketizer> _videoPacketizer;
	//	std::shared_ptr<Media::AbstractDecoder> _videoDecoder;
	//	std::shared_ptr<Media::RawPipe> _videoRaw;
	//	std::shared_ptr<Media::Video::H264Demuxer> _videoDemuxer;
	//	int _userID;
	//	friend class Service;

	//};
	class CC_NETWORK_EXPORT Service
	{
	public:
		explicit Service(std::shared_ptr<NetworkCoordinator>, rtc::Configuration config);
		//QFuture<void> establishPeerConnection(int userID);
		QFuture<void> openLocalVideo(int userID,std::shared_ptr<Media::FramePipe> input);

		std::optional<rtc::Track> openLocalAudio(int userID, std::shared_ptr<Media::FramePipe> input);
		void onRemoteVideoClosed(std::function<void(int userID)> cb);
		void onRemoteVideoOpen(std::function<void(int,std::shared_ptr<Media::FramePipe>)> cb);
		void onRemoteAudioOpen(std::function<void(int,std::shared_ptr<Media::FramePipe>)> cb);
		void closeLocalVideo(int userID);
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
		};
		void createPeerContext(int id);
		std::shared_ptr<PeerContext> getPeerContext(int id);
		std::shared_ptr<PeerContext> getOrCreatePeerContext(int id);
	private:
		std::unordered_map<int, std::shared_ptr<QPromise<void>>> _pending;
		std::unordered_map<int, std::shared_ptr<PeerContext>> _peerContexts;
		rtc::Configuration _config;
		std::shared_ptr<NetworkCoordinator> _coordinator;
		std::optional<std::function<void(int, std::shared_ptr<Media::FramePipe>)>> _videoCb;
		std::optional<std::function<void(int, std::shared_ptr<Media::FramePipe>)>> _audioCb;
		std::optional<std::function<void(int)>> _closeVideoCb;
		std::shared_ptr<Media::Video::AbstractEncoder> _videoEncoder;
		std::shared_ptr<Media::Audio::AbstractEncoder> _audioEncoder;
		std::shared_ptr<Media::RtpPacketizer> _videoPacketizer;
		std::shared_ptr<Media::RtpPacketizer> _audioPacketizer;


	};
}
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::GatheringState& s);
//CC_NETWORK_EXPORT QDebug operator<<(QDebug debug, const rtc::PeerConnection::State& s);