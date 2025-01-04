#include "microphoneinput.h"

using namespace media;

MicrophoneInput::MicrophoneInput(const Device& dev)
	:out(createPacketPipe())
    ,ctx(createFormatContext())
	,device(dev)
{

}
std::shared_ptr<media::PacketPipe> MicrophoneInput::output()
{
	return out;
}
std::optional<Audio::Source> MicrophoneInput::open()
{
    if (isOpened())
        return std::nullopt;
    std::string devicestr = "audio=" + device.dsc;
    auto tempCtx = ctx.get();
    if (avformat_open_input(&tempCtx, devicestr.c_str(), av_find_input_format(media::getPlatformDeviceName()), nullptr) != 0) {
        qWarning() << "Couldn't open input:" << device.dsc;
        return std::nullopt;
    }
    int audioStream = -1;

    if (avformat_find_stream_info(ctx.get(), nullptr) < 0)
    {
        qWarning() << "Couldn't find micro streams:" << device.dsc;
        avformat_close_input(&tempCtx);
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
        qWarning() << "Couldn't find micro audio stream:" << device.dsc;
        avformat_close_input(&tempCtx);
        return std::nullopt;
    }
    active.store(true, std::memory_order_seq_cst);
    micThread = std::thread(&MicrophoneInput::threadFunc, this);
    Audio::Source out;
    av_dump_format(ctx.get(), 0, "", 0);
    auto cp = ctx->streams[audioStream]->codecpar;
    out.codecID = cp->codec_id;
    out.channelCount = cp->channels;
    out.format = (AVSampleFormat)cp->format;
    out.par = cp;
    return out;
}
bool MicrophoneInput::isOpened()
{
	return false;
}
void MicrophoneInput::close()
{
}
std::vector<MicrophoneInput::Device> MicrophoneInput::availableDevices()
{
    std::vector<MicrophoneInput::Device> out;

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
        return out;
    }
    for (size_t i = 0; i < dlist->nb_devices; i++)
    {
        for (size_t j = 0; j < dlist->devices[i]->nb_media_types; j++)
        {
            if (dlist->devices[i]->media_types[j] == AVMEDIA_TYPE_AUDIO)
            {
                out.push_back({ dlist->devices[i]->device_name,dlist->devices[i]->device_description });
            }
        }
    }
    return out;
}
MicrophoneInput::Device MicrophoneInput::currentDevice() const
{
	return device;
}
MicrophoneInput::~MicrophoneInput()
{
    close();
}
std::shared_ptr<AVFormatContext> MicrophoneInput::context()
{
    return ctx;
}
void MicrophoneInput::threadFunc()
{
    while (active.load(std::memory_order_relaxed))
    {
        auto packet = out->holdForWriting();
        av_packet_unref(packet.ptr.get());
        if (av_read_frame(ctx.get(), packet.ptr.get()) < 0)
        {
            out->unmapWriting(packet.subpipe, false);
            qWarning() << ("mic error");
            return;
        }
        out->unmapWriting(packet.subpipe, true);

    }
}