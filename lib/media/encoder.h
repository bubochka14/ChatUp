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

#include "Processing.h"

extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libavutil/audio_fifo.h"
#include <libswscale/swscale.h>
}
#include "CodecSpecificInfo.h"
Q_DECLARE_LOGGING_CATEGORY(LC_ENCODER);
struct EncodedImage
{
	
};
namespace chatup {
	class CC_MEDIA_EXPORT FFmpegEncoder
	{
	public:
		explicit FFmpegEncoder(const AVCodec* codec);
		std::shared_ptr<PacketPipe> EncodedOutput();
		bool IsEncoding() const;
		virtual bool Encode(std::shared_ptr<AVCodecParameters> codecParameters, std::shared_ptr<FramePipe> input);
		virtual void Close();
		virtual ~FFmpegEncoder();
	protected:

		uint64_t dts();
		uint64_t pts();
		void setPTS(uint64_t other);
		void setDTS(uint64_t other);
		const AVCodec* codec();
		void setCodecContext(std::shared_ptr<AVCodecContext> other);
		virtual void fillContext(std::shared_ptr<AVCodecContext> ctx);

	private:
		const AVCodec* m_codec;

		std::atomic<bool> m_isEncoding = { false };
		std::shared_ptr<PacketPipe> m_encodedOutput;
		std::shared_ptr<AVCodecContext> m_codecContext;
		FFmpegProcessing m_processing;
		std::shared_ptr<AVFrame> m_transformedFrame;

		uint64_t _pts;
		uint64_t _dts;
		FrameListener m_frameLister;

	};
	class CC_MEDIA_EXPORT VideoEncoder : public FFmpegEncoder
	{
	public:
		bool start(std::shared_ptr<FramePipe> input);
		std::shared_ptr<FramePipe> input();
		void Close();
		SourceConfig config();
		VideoEncoder(SourceConfig config,const AVCodec* cdc);
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
	class CC_MEDIA_EXPORT AudioEncoder : public FFmpegEncoder
	{
	public:
		bool start(std::shared_ptr<FramePipe> input);
		void Close() override;
		SourceConfig config();
		AudioEncoder(SourceConfig config, const AVCodec* cdc);
	protected:
		bool checkSampleFormat(AVSampleFormat fmt);
		bool checkSampleRate(int rate);
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;
	private:
		bool encodeFrame(AVFrame* fr);
		Audio::SourceConfig _config;
		std::shared_ptr<FramePipe> _converted;
		std::shared_ptr<SwrContext> _swr;
		std::shared_ptr<FramePipe> _input;
		std::optional<int> _listenerIndex;
		int _samplesConverted;
		uint8_t* _chunkBegin;
		int chunkOffset;
	};
	class CC_MEDIA_EXPORT OpusEncoder final : public AudioEncoder
	{
	public:
		OpusEncoder(SourceConfig config);
	protected:
		void fillContext(std::shared_ptr<AVCodecContext> ctx) override;

	};
}
