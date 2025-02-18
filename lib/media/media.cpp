#include "media.h"
using namespace Media;
Video::QtVideoBuffer::QtVideoBuffer()
    :frame(nullptr)
{}
void Video::QtVideoBuffer::setFrame(std::shared_ptr<AVFrame> fr)
{
    frame = fr;
}
QVideoFrameFormat Video::QtVideoBuffer::format() const
{
    return frame? QVideoFrameFormat({ frame->width,frame->height }, toQtPixel((AVPixelFormat)frame->format))
        : QVideoFrameFormat({ 0,0 }, QVideoFrameFormat::Format_Invalid);

}
QAbstractVideoBuffer::MapData Video::QtVideoBuffer::map(QVideoFrame::MapMode mapMode)
{
    MapData out;
    if (!frame)
        return out;
    out.planeCount = av_pix_fmt_count_planes((AVPixelFormat)frame->format);
    for (size_t i = 0; i < 3; i++)
    {
        out.bytesPerLine[i] = frame->linesize[i];
        out.dataSize[i] = frame->linesize[i];
        out.data[i] = frame->data[i];
    }
    return out;

}
QVideoFrameFormat::PixelFormat Media::Video::toQtPixel(AVPixelFormat avPixelFormat)
{
    if (avPixelFormat < 0)
        return QVideoFrameFormat::Format_Invalid;
    switch (avPixelFormat) {
    default:
        break;
    case AV_PIX_FMT_NONE:
        Q_ASSERT(!"Invalid avPixelFormat!");
        return QVideoFrameFormat::Format_Invalid;
    case AV_PIX_FMT_ARGB:
        return QVideoFrameFormat::Format_ARGB8888;
    case AV_PIX_FMT_0RGB:
        return QVideoFrameFormat::Format_XRGB8888;
    case AV_PIX_FMT_BGRA:
        return QVideoFrameFormat::Format_BGRA8888;
    case AV_PIX_FMT_BGR0:
        return QVideoFrameFormat::Format_BGRX8888;
    case AV_PIX_FMT_ABGR:
        return QVideoFrameFormat::Format_ABGR8888;
    case AV_PIX_FMT_0BGR:
        return QVideoFrameFormat::Format_XBGR8888;
    case AV_PIX_FMT_RGBA:
        return QVideoFrameFormat::Format_RGBA8888;
    case AV_PIX_FMT_RGB0:
        return QVideoFrameFormat::Format_RGBX8888;
    case AV_PIX_FMT_YUVJ422P:
        return QVideoFrameFormat::Format_YUV422P;
    case AV_PIX_FMT_YUV422P:
        return QVideoFrameFormat::Format_YUV422P;
    case AV_PIX_FMT_YUV420P:
        return QVideoFrameFormat::Format_YUV420P;
    case AV_PIX_FMT_YUV420P10:
        return QVideoFrameFormat::Format_YUV420P10;
    case AV_PIX_FMT_UYVY422:
        return QVideoFrameFormat::Format_UYVY;
    case AV_PIX_FMT_YUYV422:
        return QVideoFrameFormat::Format_YUYV;
    case AV_PIX_FMT_NV12:
        return QVideoFrameFormat::Format_NV12;
    case AV_PIX_FMT_NV21:
        return QVideoFrameFormat::Format_NV21;
    case AV_PIX_FMT_GRAY8:
        return QVideoFrameFormat::Format_Y8;
    case AV_PIX_FMT_GRAY16:
        return QVideoFrameFormat::Format_Y16;
    case AV_PIX_FMT_P010:
        return QVideoFrameFormat::Format_P010;
    case AV_PIX_FMT_P016:
        return QVideoFrameFormat::Format_P016;
    case AV_PIX_FMT_MEDIACODEC:
        return QVideoFrameFormat::Format_SamplerExternalOES;
    }
    const AVPixFmtDescriptor* descriptor = av_pix_fmt_desc_get(avPixelFormat);

    if (descriptor->flags & AV_PIX_FMT_FLAG_RGB)
        return QVideoFrameFormat::Format_RGBA8888;

    if (descriptor->comp[0].depth > 8)
        return QVideoFrameFormat::Format_P016;
    return QVideoFrameFormat::Format_YUV420P;
}
Video::SinkConnector::~SinkConnector()
{
    input->removeListener(listenerIndex);
    //flush sink
    current.qframe = QVideoFrame();
    prev.qframe = QVideoFrame();
    sink->setVideoFrame(QVideoFrame());
}
Video::SinkConnector::SinkConnector(std::shared_ptr<FramePipe> pipe, QVideoSink* s)
{
    current.buf = new QtVideoBuffer();
    prev.buf = new QtVideoBuffer();
    input = std::move(pipe);
    sink = s;
    listenerIndex = input->onDataChanged([this](std::shared_ptr<AVFrame> frame, size_t index)
        {
            current.buf->setFrame(frame);
            current.pipeIndex = index; 
            if (!current.qframe.has_value())
                //QVideoFrame takes ownership of QtVideoBuffer  
                current.qframe = QVideoFrame(current.buf, current.buf->format());
            sink->setVideoFrame(current.qframe.value());
            if (prev.pipeIndex != -1)
            {
                input->unmapReading(prev.pipeIndex);
            }
            std::swap(prev, current);

        });
}