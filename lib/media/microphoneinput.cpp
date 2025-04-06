#include "microphoneinput.h"
using namespace Media::Audio;
Q_LOGGING_CATEGORY(LC_MICROPHONE, "Microphone");

Microphone::Microphone(std::string dev)
	:out(createPacketPipe())
    ,device(std::move(dev))
    ,ctx(nullptr)
{

}
std::shared_ptr<Media::PacketPipe> Microphone::output()
{
	return out;
}
std::optional<SourceConfig> Microphone::open()
{
    if (isOpened())
        return std::nullopt;
    std::string devicestr = "audio=" + device;;
    if (avformat_open_input(&ctx, devicestr.c_str(), av_find_input_format(Media::getPlatformDeviceName()), nullptr) != 0) {
        qCWarning(LC_MICROPHONE) << "Couldn't open input:" << device;
        return std::nullopt;
    }
    int audioStream = -1;

    if (avformat_find_stream_info(ctx, nullptr) < 0)
    {
        qCWarning(LC_MICROPHONE) << "Couldn't find micro streams:" << device;
        avformat_close_input(&ctx);
        return std::nullopt;
    }
    for (size_t i = 0; i < ctx->nb_streams; i++)
    {
        if (ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioStream = i; break;
        }
    }
    if (audioStream == -1)
    {
        qCWarning(LC_MICROPHONE) << "Couldn't find micro audio stream:" << device;
        avformat_close_input(&ctx);
        return std::nullopt;
    }
    active.store(true, std::memory_order_seq_cst);
    micThread = std::thread(&Microphone::threadFunc, this);
    Audio::SourceConfig out;
    out.par = avcodec_parameters_alloc();//leak
    av_dump_format(ctx, 0, "", 0);
    avcodec_parameters_copy(out.par, ctx->streams[audioStream]->codecpar);
    out.name = device;
    return out;
}
bool Microphone::isOpened()
{
	return false;
}
void Microphone::close()
{
    active.store(false, std::memory_order_seq_cst);
    if (micThread.joinable())
        micThread.join();
    if(ctx)
    {
        avformat_close_input(&ctx);
    }
}
std::vector<std::string> Microphone::availableDevices()
{
    Media::Init();
    std::vector<std::string> out;

    const AVInputFormat* ifmt = av_find_input_format(Media::getPlatformDeviceName());
    if (!ifmt)
    {
        qCCritical(LC_MICROPHONE) << "Cannot find platform device: " << Media::getPlatformDeviceName();
        return out;
    }
    AVDeviceInfoList* dlist = nullptr;
    int size = avdevice_list_input_sources(ifmt, "", nullptr, &dlist);
    if (!dlist)
    {
        return out;
    }
    for (size_t i = 0; i < dlist->nb_devices; i++)
    {
        for (size_t j = 0; j < dlist->devices[i]->nb_media_types; j++)
        {
            if (dlist->devices[i]->media_types[j] == AVMEDIA_TYPE_AUDIO)
            {
                out.push_back(dlist->devices[i]->device_description);
            }
        }
    }
    return out;
}
std::string Microphone::currentDevice() const
{
	return device;
}
Microphone::~Microphone()
{
    close();
}
//AVFormatContext* Microphone::context()
//{
//    return ctx;
//}
void Microphone::threadFunc()
{
    while (active.load(std::memory_order_relaxed))
    {
        auto packet = out->tryHoldForWriting();
        if (!packet.has_value())
        {
            qCWarning(LC_MICROPHONE) << "Output pipe overflow";
            return;
        }
        av_packet_unref(packet->ptr.get());
        if (int ret = av_read_frame(ctx, packet->ptr.get()) < 0)
        {
            out->unmapWriting(packet->subpipe, false);
            qCWarning(LC_MICROPHONE) << "Packet read error:" << Media::av_err2string(ret);

            return;
        }
        out->unmapWriting(packet->subpipe, true);

    }
}