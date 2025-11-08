#pragma once
#include <QObject>
#include <queue>
#include <atomic>
#include <mutex>
#include "datapipe.h"
#include "taskqueue.h"
#include "mediacontext.h"
#include "media.h"
#include "media_include.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_DECODER)
namespace Media {
	class CC_MEDIA_EXPORT AbstractDecoder : protected AbstractCodec
	{
	public:
		void close();
		std::shared_ptr<FramePipe> output();
	protected:
		explicit AbstractDecoder(const AVCodec* cdc);
		bool	 open(std::shared_ptr<PacketPipe> input);
		//bool	 fillContext(std::shared_ptr<AVCodecContext> ctx) override;
	private:
		std::optional<int> _inputListenIndex;
		std::queue<int> _holdingPackets;
		std::shared_ptr<FramePipe> _out;
		std::shared_ptr<PacketPipe> _input;
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
			Decoder(SourceConfig src);
			std::optional<SourceConfig> open(std::shared_ptr<PacketPipe> input);
		protected:
			bool fillContext(std::shared_ptr<AVCodecContext> ctx) override;
		private:
			SourceConfig _config;
		};
		class CC_MEDIA_EXPORT H264Decoder : public AbstractDecoder
		{
		public:
			H264Decoder();
			bool open(std::shared_ptr<PacketPipe> input);
		};
	}
	namespace Audio
	{
		class CC_MEDIA_EXPORT Decoder : public AbstractDecoder
		{
		public:
			Decoder(SourceConfig src);
			std::optional<SourceConfig> open(std::shared_ptr<PacketPipe> input);
		protected:
			bool fillContext(std::shared_ptr<AVCodecContext> ctx) override;
		private:
			SourceConfig _config;
		};
		class CC_MEDIA_EXPORT OpusDecoder : public AbstractDecoder
		{
		public:
			OpusDecoder();
			bool open(std::shared_ptr<PacketPipe> input);
		protected:
			bool fillContext(std::shared_ptr<AVCodecContext> ctx) override;
		};
	}
}