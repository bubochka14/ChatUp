#pragma once
#include <qobject>
#include <queue>
#include <atomic>
#include <mutex>
#include "datapipe.h"
#include "taskqueue.h"
#include "media.h"
extern "C" 
{
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
}
#include "media_include.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_DECODER)
Q_DECLARE_LOGGING_CATEGORY(LC_H264DEMUXER)
namespace Media {
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
	class CC_MEDIA_EXPORT AbstractDecoder
	{
	public:
		explicit AbstractDecoder();
		bool start(std::shared_ptr<PacketPipe> input);
		void stop();
		//bool started();
		std::shared_ptr<FramePipe> output();
		virtual ~AbstractDecoder();
	protected:
		void initialize(std::shared_ptr<AVCodecContext>, const AVCodec*);
	private:
		std::atomic<bool> active = {false};
		std::optional<int> inputListenIndex;
		TaskQueue quene;
		std::shared_ptr<FramePipe> out;
		std::shared_ptr<PacketPipe> input;
		std::shared_ptr<AVCodecContext> _ctx;
		const AVCodec* _codec;

	};
	namespace Video {
		class CC_MEDIA_EXPORT Decoder : public AbstractDecoder
		{
		public:
			Decoder(const SourceConfig& src);
		};
		class CC_MEDIA_EXPORT H264Decoder : public AbstractDecoder
		{
		public:
			H264Decoder();
		};
		class CC_MEDIA_EXPORT H264Demuxer
		{
		public:
			H264Demuxer();
			void start(std::shared_ptr<Media::RawPipe>);
			std::shared_ptr<Media::PacketPipe> output();
			struct ReadingOpaque
			{
				const uint8_t* data;
				size_t size = 0;
				size_t totalWritten = 0;
			};
		private:
			AVFormatContext* _ctx;
			std::shared_ptr<Media::PacketPipe> _out;
			ReadingOpaque _readingOpaque;

		};
	}
	namespace Audio
	{
		class CC_MEDIA_EXPORT Decoder : public AbstractDecoder
		{
		public:
			Decoder(const SourceConfig& src);
		};
	}
}