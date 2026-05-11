#include "Processing.h"

using namespace chatup;
FFmpegRescaler::FFmpegRescaler()
{
}

bool FFmpegRescaler::Rescale(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> rescaledFrame, int width, int height, AVPixelFormat format)
{

	if (!m_swsContext 
		|| inputFrame->height != m_sourceHeight
		|| inputFrame->width != m_sourceWidth
		|| inputFrame->format != m_sourceFormat
		|| m_outputWidth != width
		|| m_outputHeight != height
		|| m_outputFormat != format)
	{
		if (!InitSwsContext())
			return false;
	}
	return sws_scale(m_swsContext.get(), inputFrame->data,
		inputFrame->linesize, 0, inputFrame->height, rescaledFrame->data, rescaledFrame->linesize);
}

bool FFmpegRescaler::InitSwsContext()
{
	m_swsContext.reset(sws_getContext(m_outputWidth,
		m_sourceHeight,
		m_sourceFormat,
		m_outputWidth,
		m_outputHeight,
		m_outputFormat,
		SWS_BILINEAR,
		nullptr, nullptr, nullptr));
	if (!m_swsContext)
	{
		//qCCritical(LC_ENCODER) << "Cannot alloc SwrContext";
		return false;
	}

	return true;
}

FFmpegProcessing::ProcessStatus FFmpegProcessing::ProcessFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame, AVMediaType type)
{
	if (type == AVMEDIA_TYPE_VIDEO)
		return ProcessVideoFrame(std::move(inputFrame), std::move(outFrame));
	if (type == AVMEDIA_TYPE_AUDIO)
		return ProcessAudioFrame(std::move(inputFrame), std::move(outFrame));
	return ProcessStatus::FrameOk;
}

FFmpegProcessing::ProcessStatus FFmpegProcessing::ProcessVideoFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame)
{
	Resolution resolution;
	AVPixelFormat format = static_cast<AVPixelFormat>(inputFrame->format);
	if (m_outResolution.has_value())
	{
		resolution = m_outResolution.value();
	}
	else
	{
		resolution.m_height = inputFrame->height;
		resolution.m_width = inputFrame->width;
	}
	if (!m_supportedPixFormats.empty() && !m_supportedPixFormats.contains(format))
	{
		format = *m_supportedPixFormats.begin();
	}
	if (inputFrame->width == resolution.m_width && inputFrame->height == resolution.m_height && inputFrame->format == format)
		return ProcessStatus::FrameOk;

	if (outFrame->width != resolution.m_width
		|| outFrame->height != resolution.m_height
		|| outFrame->format != format)
	{
		auto* rawFrame = outFrame.get();
		av_frame_free(&rawFrame);
		outFrame->width = resolution.m_width;
		outFrame->height = resolution.m_height;
		outFrame->format = format;
		if (av_frame_get_buffer(outFrame.get(), 32) < 0)
		{
			//qCCritical(LC_ENCODER) << "Error while creating rescaled frame buffer:" << av_err2string(ret);
			return ProcessStatus::Error;
		}

	}
	if (!m_rescaler)
		m_rescaler = std::make_unique<FFmpegRescaler>();
	auto ret = m_rescaler->Rescale(inputFrame, outFrame, resolution.m_width, resolution.m_height, format);
	return ret ? ProcessStatus::FrameRewritten : ProcessStatus::Error;
	
}

bool FFmpegProcessing::ProcessAudioFrame(std::shared_ptr<AVFrame> inputFrame, std::shared_ptr<AVFrame> outFrame)
{
}
