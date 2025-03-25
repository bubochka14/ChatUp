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

namespace Media::Video {
	class CC_MEDIA_EXPORT AbstractEncoder
	{
	public:
		std::shared_ptr<Media::PacketPipe> output();
		bool start(std::shared_ptr<Media::FramePipe> input, Media::Video::SourceConfig config);
		std::shared_ptr<AVCodecContext> codecContext();
		std::shared_ptr<Media::FramePipe> input();
		void close();
		bool isStarted();
		virtual ~AbstractEncoder();
	protected:
		explicit AbstractEncoder();
		void initialize(std::shared_ptr<AVCodecContext>, const AVCodec*);

	private:
		bool checkPixelFormat(AVPixelFormat check);
		size_t _pts;
		size_t _dts;
		std::atomic<bool> _isStarted = { false };
		std::shared_ptr<Media::PacketPipe> _out;
		std::shared_ptr<AVCodecContext> _cCtx;
		const AVCodec* _cdc;
		std::shared_ptr<FramePipe> _input;
		std::shared_ptr<SwsContext> _sws;
		std::shared_ptr<AVFrame> _rescaledFrame;
		std::optional<int> _listenerIndex;
	};
	class CC_MEDIA_EXPORT H264Encoder final : public AbstractEncoder
	{
	public:
		H264Encoder();
	};
}
namespace Media::Audio {
	class CC_MEDIA_EXPORT AbstractEncoder
	{
	public:
		std::shared_ptr<Media::PacketPipe> output();
		bool start(std::shared_ptr<Media::FramePipe> input, Media::Audio::SourceConfig config);
		std::shared_ptr<AVCodecContext> codecContext();
		std::shared_ptr<FramePipe> input();
		void close();
		bool isStarted();
		virtual ~AbstractEncoder();
	protected:
		explicit AbstractEncoder();
		void initialize(std::shared_ptr<AVCodecContext>, const AVCodec*);
		bool checkSampleFormat(AVSampleFormat fmt);
		bool checkSampleRate(int rate);
		AVFrame makeFrameChunk(std::shared_ptr<AVFrame> src, size_t offset, size_t samples);
	private:
		struct SamplesPack
		{
			int totalSamples;
			int encodedCount;
			int framePipeIndex;
			float* data;
		};
		using SamplePipe = Media::DataPipe<2,SamplesPack>;
		size_t _pts;
		size_t _dts;
		std::atomic<bool> _isStarted = { false };
		std::shared_ptr<FramePipe> _converted;
		std::shared_ptr<Media::PacketPipe> _out;
		std::shared_ptr<AVCodecContext> _cCtx;
		std::shared_ptr<SwrContext> _swr;
		std::shared_ptr<FramePipe> _input;
		std::optional<int> _listenerIndex;
		const AVCodec* _cdc;

	};
	class CC_MEDIA_EXPORT AACEncoder final : public AbstractEncoder
	{
	public:
		AACEncoder();
	};
	class CC_MEDIA_EXPORT OpusEncoder final : public AbstractEncoder
	{
	public:
		OpusEncoder();
	};
}
