#include "camerainput.h"
using namespace media;
CameraInput::CameraInput(Config conf)
    :ictx(avformat_alloc_context())
    ,device(std::move(conf))
    ,out(std::make_shared<PacketPipe>([]() 
        {return av_packet_alloc(); },
        [](AVPacket* p)
        {av_packet_free(&p); }
    
    ))
    ,active(false)
{
   

}
std::vector<CameraInput::Config> CameraInput::availableDevices()
{
    std::vector<CameraInput::Config> out;

    const AVInputFormat* ifmt = av_find_input_format(media::getPlatformDeviceName());
    if (!ifmt)
    {
        qCritical() << "Cannot find platform device: " << media::getPlatformDeviceName();
        return out;
    }
    AVDeviceInfoList* dlist = nullptr;
    int size = avdevice_list_input_sources(ifmt, "", nullptr, &dlist);
    if (!dlist)
    {
        qDebug() << "hola";

        return out;
    }
    qDebug() << "hi";
    for (size_t i = 0; i < dlist->nb_devices; i++)
    {
        for (size_t j = 0; j < dlist->devices[i]->nb_media_types; j++)
        {
            if (dlist->devices[i]->media_types[j] == AVMEDIA_TYPE_VIDEO)
            {
                out.push_back({ dlist->devices[i]->device_name,dlist->devices[i]->device_description });
            }
        }
    }
    return out;
}

std::shared_ptr<media::PacketPipe> CameraInput::output()
{
    return out;
}
bool CameraInput::isOpened()
{
    return active.load(std::memory_order_seq_cst);
}

std::optional<Video::Source> CameraInput::open()
{
    if (isOpened())
        return std::nullopt;
    std::string devicestr = "video="+ device.dsc;
    if (avformat_open_input(&ictx, devicestr.c_str(), av_find_input_format(media::getPlatformDeviceName()), nullptr) != 0) {
        qWarning() << "Couldn't open input:" << device.dsc;
        return std::nullopt;
    }
    int videoStreamIndex = -1;

    if (avformat_find_stream_info(ictx, nullptr) < 0)
    {
        qWarning() << "Couldn't find camera streams:" << device.dsc;
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
        qWarning() << "Couldn't find camera video stream:" << device.dsc;
        avformat_close_input(&ictx);
        return std::nullopt;
    }
    active.store(true, std::memory_order_seq_cst);
    cameraThread = std::thread(&CameraInput::threadFunc, this);
    Video::Source out;
    auto cp = ictx->streams[videoStreamIndex]->codecpar;
    out.codecID = cp->codec_id;
    out.height  = cp->height;
    out.width   = cp->width;
    out.format  = (AVPixelFormat)cp->format;
    av_dump_format(ictx, 0, "", 0);
    return out;
}
void CameraInput::close()
{
    avformat_close_input(&ictx);
    active.store(true, std::memory_order_seq_cst);
    cameraThread.join();
}
CameraInput::~CameraInput()
{
    close();
}
AVFormatContext* CameraInput::ctx()
{
    return ictx;
}
void CameraInput::threadFunc()
{
    while (active.load(std::memory_order_relaxed))
    {
        auto packet = out->holdForWriting();
        av_packet_unref(packet.ptr.get());
        if (av_read_frame(ictx, packet.ptr.get()) < 0)
        {
            out-> unmapWriting(packet.subpipe,false);
            qWarning() <<  ("cam error");
            return;
        }
        out->unmapWriting(packet.subpipe,true);

    }
}
