#pragma once
#include "media_include.h"
#include "vector"
#include <string>
#include <memory>
#include <media.h>
#include <qloggingcategory.h>

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
		virtual bool start(std::shared_ptr<PacketPipe> input);
	private:
		AVFormatContext* _packetizationCxt;
		std::shared_ptr<RawPipe> _output;
		PacketizationConfig _config;
	};
}