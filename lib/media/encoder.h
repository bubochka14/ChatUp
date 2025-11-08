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
#include <QtConcurrent/qtconcurrentrun.h>
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libavutil/audio_fifo.h"
#include <libswscale/swscale.h>
}
Q_DECLARE_LOGGING_CATEGORY(LC_ENCODER);
namespace Media {
	class CC_MEDIA_EXPORT AbstractEncoder
	{
	public:
		std::shared_ptr<Media::PacketPipe> output();
		std::shared_ptr<AVCodecContext> codecContext();//need to remove
		bool isStarted();
		virtual bool start(std::shared_ptr<Media::FramePipe> input);
		virtual void close();
		virtual ~AbstractEncoder();
	protected:
		AbstractEncoder(const AVCodec* cdc);

		uint64_t dts();
		uint64_t pts();
		void setPTS(uint64_t other);
		void setDTS(uint64_t other);
		const AVCodec* codec();
		void setCodecContext(std::shared_ptr<AVCodecContext> other);
		virtual void fillContext(std::shared_ptr<AVCodecContext> ctx);

	private:
		std::atomic<bool> _isStarted = { false };
		std::shared_ptr<Media::PacketPipe> _out;
		std::shared_ptr<AVCodecContext> _cCtx;
		const AVCodec* _cdc;
		uint64_t _pts;
		uint64_t _dts;

	};
}
namespace Media::Video {
	class CC_MEDIA_EXPORT Encoder : public AbstractEncoder
	{
	public:
		bool start(std::shared_ptr<Media::FramePipe> input);
		std::shared_ptr<Media::FramePipe> input();
		void close();
		SourceConfig config();
		Encoder(SourceConfig config,const AVCodec* cdc);
	protected:
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;

	private:
		bool checkPixelFormat(AVPixelFormat format);
		SourceConfig _config;
		std::shared_ptr<FramePipe> _input;
		std::shared_ptr<SwsContext> _sws;
		std::shared_ptr<AVFrame> _rescaledFrame;
		std::optional<int> _listenerIndex;
	};

	class CC_MEDIA_EXPORT H264Encoder final : public Encoder
	{
	public:
		H264Encoder(Media::Video::SourceConfig config);
	protected:
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;

	};
}
namespace Media::Audio {
	class CC_MEDIA_EXPORT Encoder : public AbstractEncoder
	{
	public:
		bool start(std::shared_ptr<Media::FramePipe> input);
		void close() override;
		SourceConfig config();
		Encoder(SourceConfig config, const AVCodec* cdc);
	protected:
		bool checkSampleFormat(AVSampleFormat fmt);
		bool checkSampleRate(int rate);
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;
	private:
		bool encodeFrame(AVFrame* fr);
		Media::Audio::SourceConfig _config;
		std::shared_ptr<FramePipe> _converted;
		std::shared_ptr<SwrContext> _swr;
		std::shared_ptr<FramePipe> _input;
		std::optional<int> _listenerIndex;
		int _samplesConverted;
		uint8_t* _chunkBegin;
		int chunkOffset;
	};
	class CC_MEDIA_EXPORT AACEncoder final : public Encoder
	{
	public:
		AACEncoder(SourceConfig config);
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;

	};
	class CC_MEDIA_EXPORT OpusEncoder final : public Encoder
	{
	public:
		OpusEncoder(SourceConfig config);
	protected:
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;

	};
}
