#pragma once
#include <qobject>
#include <queue>
#include <atomic>
#include <mutex>
#include "datapipe.h"
#include <qdebug.h>
#include "taskqueue.h"
#include "media.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include "media_include.h"
class CC_MEDIA_EXPORT DataBuffer
{
public:
	using Binary = std::vector<std::byte>;
	DataBuffer():byteOffset(0) {}
	bool empty();
	void pushData(Binary bin);
	int popData(uint8_t* prt, int size);
public:
	std::deque <Binary> data;
	std::mutex mutex;
	size_t byteOffset;
};
namespace media {
	class CC_MEDIA_EXPORT AbstractDecoder
	{
	public:
		explicit AbstractDecoder();
		virtual bool start(std::shared_ptr<PacketPipe> input);
		virtual void stop();
		//bool started();
		std::shared_ptr<media::FramePipe> output();
		virtual ~AbstractDecoder();
	protected:
		virtual const AVCodec* codec() = 0;
		virtual const std::shared_ptr<AVCodecContext> codexContext() = 0;
	private:
		std::atomic<bool> active = {false};
		TaskQueue quene;
		std::shared_ptr<FramePipe> out;

	};
	namespace Video {
		class CC_MEDIA_EXPORT Decoder : public AbstractDecoder
		{
		public:
			Decoder(const Video::Source& src);
		protected:
			const AVCodec* codec() override;
			const std::shared_ptr<AVCodecContext> codexContext() override;
		private:
			const AVCodec* cdc;
			std::shared_ptr<AVCodecContext> cCtx;
		};
	}
	namespace Audio
	{
		class CC_MEDIA_EXPORT Decoder : public AbstractDecoder
		{
		public:
			Decoder(const Audio::Source& src);
		protected:
			const AVCodec* codec() override;
			const std::shared_ptr<AVCodecContext> codexContext() override;
		private:
			const AVCodec* cdc;
			std::shared_ptr<AVCodecContext> cCtx;
		};
	}
}