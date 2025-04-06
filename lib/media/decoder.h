#pragma once
#include <qobject>
#include <queue>
#include <atomic>
#include <mutex>
#include "datapipe.h"
#include "taskqueue.h"
#include <qstack>
#include "media.h"
extern "C" 
{
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
}
#include "media_include.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_DECODER)
Q_DECLARE_LOGGING_CATEGORY(LC_H264DEMUXER)
namespace Media {
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
		void initialize(std::shared_ptr<AVCodecContext>, const AVCodec*, std::shared_ptr<FramePipe>);
	private:
		std::atomic<bool> active = {false};
		std::optional<int> inputListenIndex;
		std::shared_ptr<FramePipe> _out;
		std::shared_ptr<PacketPipe> _input;
		std::shared_ptr<AVCodecContext> _ctx;
		QStack<int> _holdingIndex;
		const AVCodec* _codec;
		std::mutex _decodeMutex;
		std::shared_ptr<AVFrame> _drainFrame;
		QThreadPool _pool;
		int64_t delta = 0;
		int64_t timestamp = 0;
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
		class CC_MEDIA_EXPORT OpusDecoder : public AbstractDecoder
		{
		public:
			OpusDecoder();
		};
		class CC_MEDIA_EXPORT OpusDemuxer
		{
		public:
			OpusDemuxer();
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
}