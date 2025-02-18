#pragma once
#include <functional>
#include <qobject.h>
#include "datapipe.h"
#include <thread>
#include <mutex>
#include "media.h"
#include "taskqueue.h"
#include <condition_variable>
#include <qloggingcategory.h>
extern "C"
{
#include <libavcodec/avcodec.h>
}
Q_DECLARE_LOGGING_CATEGORY(LC_ENCODER);
namespace Media::Video {
	class CC_MEDIA_EXPORT AbstractEncoder
	{
	public:
		std::shared_ptr<Media::PacketPipe> output();
		bool start(std::shared_ptr<Media::FramePipe> input, SourceConfig source);
		virtual ~AbstractEncoder() = default;
	protected:
		explicit AbstractEncoder();
		virtual std::shared_ptr<AVCodecContext> codecContext() = 0;
		virtual const AVCodec* codec() = 0;
	public:
		size_t _pts;
		size_t _dts;
		TaskQueue queue;
		std::shared_ptr<Media::PacketPipe> _out;

	};
	class CC_MEDIA_EXPORT H264Encoder final : public AbstractEncoder 
	{
	public:
		H264Encoder();
		std::shared_ptr<AVCodecContext> codecContext() override;
		const AVCodec* codec() override;
	private:
		const AVCodec* _codec;
		std::shared_ptr<AVCodecContext> _ctx;

	};

}