#include "camerainput.h"
using namespace Media::Video;
Q_LOGGING_CATEGORY(LC_CAMERA, "Camera");
Camera::Camera(std::string dev)
    :ictx(nullptr)
    ,device(std::move(dev))
    ,out(Media::createPacketPipe())
    ,active(false)
{
    Media::Init();

}
std::vector<std::string> Camera::availableDevices()
{
    Media::Init();

    std::vector<std::string> out;

    const AVInputFormat* ifmt = av_find_input_format(Media::getPlatformDeviceName());
    if (!ifmt)
    {
        qCCritical(LC_CAMERA) << "Cannot find platform device: " << Media::getPlatformDeviceName();
        return out;
    }
    AVDeviceInfoList* dlist = nullptr;
    int size = avdevice_list_input_sources(ifmt, "", nullptr, &dlist);
    for (size_t i = 0; i < dlist->nb_devices; i++)
    {
        for (size_t j = 0; j < dlist->devices[i]->nb_media_types; j++)
        {
            if (dlist->devices[i]->media_types[j] == AVMEDIA_TYPE_VIDEO)
            {
                out.push_back(dlist->devices[i]->device_description);
            }
        }
    }
    return out;
}

std::shared_ptr<Media::PacketPipe> Camera::output()
{
    return out;
}
bool Camera::isOpened()
{
    return active.load(std::memory_order_seq_cst);
}

std::optional<SourceConfig> Camera::open()
{
    if (isOpened())
        return std::nullopt;
    std::string devicestr = "video="+ device;
    if (avformat_open_input(&ictx, devicestr.c_str(), av_find_input_format(Media::getPlatformDeviceName()), nullptr) != 0) {
        qCWarning(LC_CAMERA) << "Couldn't open input:" << device;
        return std::nullopt;
    }
    int videoStreamIndex = -1;

    if (avformat_find_stream_info(ictx, nullptr) < 0)
    {
        qCWarning(LC_CAMERA) << "Couldn't find camera streams:" << device;
        avformat_close_input(&ictx);
        return std::nullopt;
    }
    for (size_t i = 0; i < ictx->nb_streams; i++)
    {
        if (ictx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i; break;
        }
    }
    if (videoStreamIndex == -1)
    {
        qCWarning(LC_CAMERA) << "Couldn't find camera video stream:" << device;
        avformat_close_input(&ictx);
        return std::nullopt;
    }
    active.store(true, std::memory_order_seq_cst);
    cameraThread = std::thread(&Camera::threadFunc, this);
    Video::SourceConfig out;
    auto cp = ictx->streams[videoStreamIndex]->codecpar;
    out.codecID = cp->codec_id;
    out.height  = cp->height;
    out.width   = cp->width;
    out.format  = (AVPixelFormat)cp->format;
    out.name = device;
    av_dump_format(ictx, 0, "", 0);
    return out;
}
void Camera::close()
{
    active.store(false, std::memory_order_seq_cst);
    if (cameraThread.joinable())
        cameraThread.join();
    if(ictx)
        avformat_close_input(&ictx);
}
Camera::~Camera()
{
    close();
}
AVFormatContext* Camera::ctx()
{
    return ictx;
}
void Camera::threadFunc()
{
    while (active.load(std::memory_order_relaxed))
    {
        auto packet = out->tryHoldForWriting();
        if (!packet.has_value())
        {
            qCWarning(LC_CAMERA) << "Output pipe overflow";
            return;
        }
        av_packet_unref(packet->ptr.get());
        if (int ret = av_read_frame(ictx, packet->ptr.get()) < 0)
        {
            out->unmapWriting(packet->subpipe,false);
            qCWarning(LC_CAMERA) <<  "Packet read error:"<< Media::av_err2string(ret);
        }
        out->unmapWriting(packet->subpipe,true);

    }
}
