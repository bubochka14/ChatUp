#include "ffmpegcameracapture.h"
#include <boost/scope_exit.hpp>
using namespace chatup;

FFmpegCameraCapture::FFmpegCameraCapture(std::shared_ptr<io_context> context)
	: MediaCaptureBase(createPacketPipe())
	, m_asioContext(std::move(context))
{
}

FFmpegCameraCapture::~FFmpegCameraCapture()
{
    Close();
}

std::optional<chatup::SourceConfig> FFmpegCameraCapture::OpenInternal(const MediaDevice& device)
{
    std::string_view deviceName = "video=" + device.m_name;
    auto* rawFormatContext = m_formatContext.get();
    if (avformat_open_input(&rawFormatContext, deviceName.data(), av_find_input_format(getPlatformDeviceName()), nullptr) != 0) {
        //qCWarning(LC_FFmpegCameraCapture) << "Couldn't open input:" << device;
        return std::nullopt;
    }
    BOOST_SCOPE_EXIT(rawFormatContext) {
        avformat_close_input(&rawFormatContext);
    } BOOST_SCOPE_EXIT_END

    std::optional<int> videoStreamIndex;

    if (avformat_find_stream_info(rawFormatContext, nullptr) < 0)
    {
        //qCWarning(LC_FFmpegCameraCapture) << "Couldn't find FFmpegCameraCapture streams:" << device;
        return std::nullopt;
    }
    for (size_t i = 0; i < rawFormatContext->nb_streams; i++)
    {
        if (rawFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i; break;
        }
    }
    if (!videoStreamIndex.has_value())
    {
        //qCWarning(LC_FFmpegCameraCapture) << "Couldn't find FFmpegCameraCapture video stream:" << device;
        return std::nullopt;
    }
    active.store(true, std::memory_order_seq_cst);
    FFmpegCameraCaptureThread = std::thread(&FFmpegCameraCapture::threadFunc, this);
    SourceConfig out;
    auto cp = ictx->streams[videoStreamIndex]->codecpar;
    out.codecID = cp->codec_id;
    out.height = cp->height;
    out.width = cp->width;
    out.format = (AVPixelFormat)cp->format;
    out.name = device;
    av_dump_format(ictx, 0, "", 0);
    return out;
}

void FFmpegCameraCapture::ReadFrameSpin()
{
    while (m_active.load(std::memory_order_relaxed))
    {
        auto packet = m_output->TryHoldForUploading();
        if (!packet.has_value())
        {
            //qCWarning(LC_FFmpegCameraCapture) << "Output pipe overflow";
            break;
        }
        av_packet_unref(packet->ptr.get());
        if (int ret = av_read_frame(m_formatContext.get(), packet->ptr.get()) < 0)
        {
            m_output->unmapWriting(packet->subpipe, false);
            //qCWarning(LC_FFmpegCameraCapture) << "Packet read error:" << av_err2string(ret);
        }
        m_output->unmapWriting(packet->subpipe, true);

    }
}

std::vector<MediaDevice> FFmpegCameraCapture::GetAvailableDevices() const
{
    std::vector<MediaDevice> foundDevices;
    Init();

    const AVInputFormat* inputFormat = av_find_input_format(getPlatformDeviceName());
    if (!inputFormat)
    {
        //qCCritical(LC_FFmpegCameraCapture) << "Cannot find platform device: " << getPlatformDeviceName();
        return foundDevices;
    }
    AVDeviceInfoList* deviceList = nullptr;
    if (avdevice_list_input_sources(inputFormat, "", nullptr, &deviceList) < 0)
    {
	    //log error;
    }
    foundDevices.reserve(deviceList->nb_devices);

    for (int deviceIndex = 0; deviceIndex < deviceList->nb_devices; deviceIndex++)
    {
        for (int formatIndex = 0; formatIndex < deviceList->devices[deviceIndex]->nb_media_types; formatIndex++)
        {
            if (deviceList->devices[deviceIndex]->media_types[formatIndex] == AVMEDIA_TYPE_VIDEO)
            {
				foundDevices.emplace_back(MediaDevice{ deviceList->devices[deviceIndex]->device_description });
            }
        }
    }
    return foundDevices;
}

void FFmpegCameraCapture::threadFunc()
{
    while (active.load(std::memory_order_relaxed))
    {
        auto packet = out->tryHoldForWriting();
        if (!packet.has_value())
        {
            qCWarning(LC_FFmpegCameraCapture) << "Output pipe overflow";
            return;
        }
        av_packet_unref(packet->ptr.get());
        if (int ret = av_read_frame(ictx, packet->ptr.get()) < 0)
        {
            out->unmapWriting(packet->subpipe, false);
            qCWarning(LC_FFmpegCameraCapture) << "Packet read error:" << av_err2string(ret);
        }
        out->unmapWriting(packet->subpipe, true);

    }
}
