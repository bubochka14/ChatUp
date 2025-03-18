#pragma once
#include "media_include.h"
#include "vector"
#include <string>
#include <memory>
#include <media.h>
#include <qloggingcategory.h>
#include "taskqueue.h"
#include <QtConcurrent/qtconcurrentrun.h>
Q_DECLARE_LOGGING_CATEGORY(LC_RTP_PACKETIZER)
namespace Media{
	struct PacketizationConfig
	{
		std::shared_ptr<AVCodecContext> ecnCtx = nullptr;
		size_t bufferSize = 1024;
		size_t maxPacketSize = 1024;
	};
	class CC_MEDIA_EXPORT RtpPacketizer final
	{
	public:
		RtpPacketizer(PacketizationConfig);
		std::shared_ptr<RawPipe> output();
		bool start(std::shared_ptr<PacketPipe> input);
		void stop();
		~RtpPacketizer();
	private:
		AVFormatContext* _packetizationCxt;
		std::shared_ptr<RawPipe> _output;
		std::shared_ptr<PacketPipe> _input;
		TaskQueue _queue;
		PacketizationConfig _config;
		std::optional<int> listenerIndex;
	};
}