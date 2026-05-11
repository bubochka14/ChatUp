#pragma once
#include <unordered_set>
#include <memory>
#include <optional>

#include "../../out/build/x64-release/vcpkg_installed/x64-windows/include/libavutil/pixfmt.h"


extern "C"
{
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>



#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

namespace chatup
{
	class FFmpegResampler
	{
	public:
		FFmpegResampler(std::unordered_set<AVSampleFormat> supportedFormats);

	private:
		std::shared_ptr<SwrContext> m_swrContext;
	};
	class FFmpegRescaler
	{
	public:
		FFmpegRescaler() = default;
		[[nodiscard]] bool Rescale(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> rescaledFrame, int width, int height, AVPixelFormat format);
	private:
		bool InitSwsContext();
		struct SwsContextDeleter {
			void operator()(SwsContext* ptr) const {
				if (ptr) sws_freeContext(ptr);
			}
		};
		std::unique_ptr<SwsContext, SwsContextDeleter> m_swsContext;
		int m_sourceHeight = 0;
		int m_sourceWidth = 0;
		AVPixelFormat m_sourceFormat = AV_PIX_FMT_NONE;
		int m_outputHeight = 0;
		int m_outputWidth = 0;
		AVPixelFormat m_outputFormat = AV_PIX_FMT_NONE;
	};
	class FFmpegProcessing
	{
	public:
		enum class ProcessStatus : uint8_t
		{
			FrameRewritten,
			FrameOk,
			Error
		};
		FFmpegProcessing() = default;
		void SetSupportedPixelFormats(std::unordered_set<AVPixelFormat> formats);
		void SetOutputResolution(int width, int height);

		ProcessStatus ProcessFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame, AVMediaType type);
	private:
		struct Resolution
		{
			int m_width = 0;
			int m_height = 0;
		};
		ProcessStatus ProcessVideoFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame);
		ProcessStatus ProcessAudioFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame);

		std::unordered_set<AVPixelFormat> m_supportedPixFormats;
		std::unique_ptr<FFmpegResampler> m_resampler;
		std::unique_ptr<FFmpegRescaler> m_rescaler;
		std::optional < Resolution> m_outResolution;

	};
}