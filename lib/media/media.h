#pragma once
#include <string>
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
}
#include <qvideoframeformat.h>
#include <qaudioformat.h>
#include "datapipe.h"
#include <memory>
#include <qabstractvideobuffer.h>
#include <qvideosink.h>
#include <qaudiosink.h>
#include <QFuture>
#include <QDebug>
namespace Media
{
    static bool Inited = false;
#ifdef av_err2str
#undef av_err2str
#include <string>
    av_always_inline std::string av_err2string(int errnum) {
        char str[AV_ERROR_MAX_STRING_SIZE];
        return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
    }
#define av_err2str(err) av_err2string(err).c_str()
#endif  // av_err2str
    static void Init()
    {
        if (Inited)
            return;
        avdevice_register_all();
        Inited = true;
    }
    static const constexpr char* getPlatformDeviceName()
    {
        return
#if defined(_WIN32) 
            "dshow"
#elif defined(__linux__) 
            "v4l2-ctl"
#elif defined(__APPLE__) 
            "avfoundation"
#else 
            "unknown"
#endif
            ;
    }
    struct Raw
    {
        uint8_t* raw = nullptr;
        size_t size = 0;
    };
    struct VectorPacket
    {
        std::vector<std::byte> vector;
        AVPacket* packet;
        void setData(std::vector<std::byte> data)
        {
            vector = std::move(data);
            if (av_packet_from_data(packet, (uint8_t*)vector.data(), vector.size()) < 0)
            {
                qWarning() << "Cannot fill packet";
            }
            packet->size = vector.size();
        }
    };
    using PacketPipe = DataPipe<128, AVPacket>;
    using FramePipe = DataPipe<128, AVFrame>;
    using BytePipe = DataPipe<2, std::vector<std::byte>>;
    using RawPipe = DataPipe<2, Raw>;
    using VectorPacketPipe = DataPipe<32, VectorPacket>;
    static std::shared_ptr<VectorPacketPipe> createVectorPacketPipe()
    {
        return std::make_shared<VectorPacketPipe>([]() {
            VectorPacket* out= new VectorPacket;
            out->packet = av_packet_alloc();
            return out;
            }, [](VectorPacket* p) {
                p->packet->buf = nullptr;
                delete p;
            });
    }
    static std::shared_ptr<AVPacket> createPacket()
    {
        return std::shared_ptr<AVPacket>(av_packet_alloc(), [](AVPacket* p) {av_packet_free(&p); });
    }
    static std::shared_ptr<AVFormatContext> createFormatContext()
    {
        return std::shared_ptr<AVFormatContext>(avformat_alloc_context(), [](AVFormatContext* p) {avformat_free_context(p); });
    }
    static std::shared_ptr<AVCodecContext> createCodecContext(const AVCodec* c)
    {
        return std::shared_ptr<AVCodecContext>(avcodec_alloc_context3(c), [](AVCodecContext* p) {avcodec_free_context(&p); });
    }
    CC_MEDIA_EXPORT std::shared_ptr<PacketPipe> createNullBufferPacketPipe();
    //for video
    static std::shared_ptr<FramePipe> createFramePipe(int width, int height, AVPixelFormat fmt, size_t al = 32)
    {
        return std::make_shared<FramePipe>([height, width, fmt,al]() {
            auto frame = av_frame_alloc();
            frame->height = height;
            frame->width = width;
            frame->format = fmt;
            av_frame_get_buffer(frame, al);
            return frame;
            }, [](AVFrame* p) {
            av_frame_free(&p);
            });
    }
    //for audio, does not allocate frame buffer
    static std::shared_ptr<FramePipe> createFramePipe(AVChannelLayout layout, AVSampleFormat fmt, int sampleRate)
    {
        return std::make_shared<FramePipe>([layout, fmt,sampleRate]() {
            auto frame = av_frame_alloc();
            frame->format = fmt;
            frame->ch_layout = layout;
            frame->sample_rate = sampleRate;
            return frame;
        }, [](AVFrame* p) {
            av_frame_free(&p);
        });
    }
    CC_MEDIA_EXPORT std::shared_ptr<PacketPipe> createPacketPipe();
    CC_MEDIA_EXPORT bool fillPacket(
        std::shared_ptr<AVPacket> pack,
        uint8_t* data,
        size_t dataSize
    );
    namespace Audio
    {
        struct SourceConfig
        {
            std::string name;
            AVCodecParameters* par;//replace later
        };
        //class CC_MEDIA_EXPORT SinkConnector
        //{
        //    SinkConnector();
        //    void drain();
        //    void close();
        //    void connect(std::shared_ptr<FramePipe> fr);
        //    void connect(QAudioSink* s);
        //    ~SinkConnector();
        //private:
        //    void establish();
        //    int listenerIndex;
        //    QUIo
        //    std::shared_ptr<Media::FramePipe> input;
        //};
        static QAudioFormat::SampleFormat toQtFormat(AVSampleFormat format)
        {
            switch (format) {
            case AV_SAMPLE_FMT_NONE:
            default:
                return QAudioFormat::Unknown;
            case AV_SAMPLE_FMT_U8:          ///< unsigned 8 bits
            case AV_SAMPLE_FMT_U8P:         ///< unsigned 8 bits: planar
                return QAudioFormat::UInt8;
            case AV_SAMPLE_FMT_S16:         ///< signed 16 bits
            case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits: planar
                return QAudioFormat::Int16;
            case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
            case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits: planar
                return QAudioFormat::Int32;
            case AV_SAMPLE_FMT_FLT:         ///< float
            case AV_SAMPLE_FMT_FLTP:        ///< float: planar
                return QAudioFormat::Float;
            case AV_SAMPLE_FMT_DBL:         ///< double
            case AV_SAMPLE_FMT_DBLP:        ///< double: planar
            case AV_SAMPLE_FMT_S64:         ///< signed 64 bits
            case AV_SAMPLE_FMT_S64P:        ///< signed 64 bits, planar
                // let's use float
                return QAudioFormat::Float;
            }
        }
        static AVSampleFormat toAVFormat(QAudioFormat::SampleFormat format)
        {
            switch (format) {
            case QAudioFormat::UInt8:
                return AV_SAMPLE_FMT_U8;
            case QAudioFormat::Int16:
                return AV_SAMPLE_FMT_S16;
            case QAudioFormat::Int32:
                return AV_SAMPLE_FMT_S32;
            case QAudioFormat::Float:
                return AV_SAMPLE_FMT_FLT;
            default:
                return AV_SAMPLE_FMT_NONE;
            }
        }
        class CC_MEDIA_EXPORT StreamSource : public QObject
        {
            Q_OBJECT;
        public:
            virtual std::shared_ptr<FramePipe> frameOutput() = 0;
            virtual QFuture<SourceConfig> open() = 0;
            virtual void close() = 0;
            virtual bool isOpen() = 0;
        };
    }
    namespace Video {
        CC_MEDIA_EXPORT QVideoFrameFormat::PixelFormat toQtPixel(AVPixelFormat avPixelFormat);
        static AVPixelFormat toAVPixel(QVideoFrameFormat::PixelFormat pixelFormat)
        {
            switch (pixelFormat) {
            default:
            case QVideoFrameFormat::Format_Invalid:
            case QVideoFrameFormat::Format_AYUV:
            case QVideoFrameFormat::Format_AYUV_Premultiplied:
            case QVideoFrameFormat::Format_YV12:
            case QVideoFrameFormat::Format_IMC1:
            case QVideoFrameFormat::Format_IMC2:
            case QVideoFrameFormat::Format_IMC3:
            case QVideoFrameFormat::Format_IMC4:
                return AV_PIX_FMT_NONE;
            case QVideoFrameFormat::Format_Jpeg:
                // We're using the data from the converted QImage here, which is in BGRA.
                return AV_PIX_FMT_BGRA;
            case QVideoFrameFormat::Format_ARGB8888:
                return AV_PIX_FMT_ARGB;
            case QVideoFrameFormat::Format_ARGB8888_Premultiplied:
            case QVideoFrameFormat::Format_XRGB8888:
                return AV_PIX_FMT_0RGB;
            case QVideoFrameFormat::Format_BGRA8888:
                return AV_PIX_FMT_BGRA;
            case QVideoFrameFormat::Format_BGRA8888_Premultiplied:
            case QVideoFrameFormat::Format_BGRX8888:
                return AV_PIX_FMT_BGR0;
            case QVideoFrameFormat::Format_ABGR8888:
                return AV_PIX_FMT_ABGR;
            case QVideoFrameFormat::Format_XBGR8888:
                return AV_PIX_FMT_0BGR;
            case QVideoFrameFormat::Format_RGBA8888:
                return AV_PIX_FMT_RGBA;
            case QVideoFrameFormat::Format_RGBX8888:
                return AV_PIX_FMT_RGB0;
            case QVideoFrameFormat::Format_YUV422P:
                return AV_PIX_FMT_YUV422P;
            case QVideoFrameFormat::Format_YUV420P:
                return AV_PIX_FMT_YUV420P;
            case QVideoFrameFormat::Format_YUV420P10:
                return AV_PIX_FMT_YUV420P10;
            case QVideoFrameFormat::Format_UYVY:
                return AV_PIX_FMT_UYVY422;
            case QVideoFrameFormat::Format_YUYV:
                return AV_PIX_FMT_YUYV422;
            case QVideoFrameFormat::Format_NV12:
                return AV_PIX_FMT_NV12;
            case QVideoFrameFormat::Format_NV21:
                return AV_PIX_FMT_NV21;
            case QVideoFrameFormat::Format_Y8:
                return AV_PIX_FMT_GRAY8;
            case QVideoFrameFormat::Format_Y16:
                return AV_PIX_FMT_GRAY16;
            case QVideoFrameFormat::Format_P010:
                return AV_PIX_FMT_P010;
            case QVideoFrameFormat::Format_P016:
                return AV_PIX_FMT_P016;
            case QVideoFrameFormat::Format_SamplerExternalOES:
                return AV_PIX_FMT_MEDIACODEC;
            }
        }
        struct SourceConfig
        {
            int height = 0;
            int width = 0;
            std::string name;
            AVRational aspectRatio;
            AVPixelFormat format = AV_PIX_FMT_NONE;
            AVCodecID codecID = AV_CODEC_ID_NONE;

        };
        class CC_MEDIA_EXPORT StreamSource : public QObject
        {
            Q_OBJECT;
        public:
            virtual std::shared_ptr<FramePipe> frameOutput() = 0;
            virtual QFuture<SourceConfig> open() = 0;
            virtual void close() = 0;
            virtual bool isOpen() = 0;
        };
        class CC_MEDIA_EXPORT QtVideoBuffer : public QAbstractVideoBuffer
        {
        public:
            QtVideoBuffer();
            void setFrame(std::shared_ptr<AVFrame> fr);
            QVideoFrameFormat format() const override;
            QAbstractVideoBuffer::MapData map(QVideoFrame::MapMode mapMode) override;
        private:
            std::shared_ptr<AVFrame> frame;
        };
        struct CC_MEDIA_EXPORT SinkConnector
        {
            SinkConnector();
            void drain(); 
            void close(); 
            void connect(std::shared_ptr<FramePipe> fr);
            void connect(QVideoSink* s);
            ~SinkConnector();
        private:
            void establish();
            struct FrameData
            {
                QtVideoBuffer* buf = nullptr;
                size_t pipeIndex = -1;
                std::optional<QVideoFrame> qframe;
            };
            FrameData current;
            FrameData prev;
            int listenerIndex;
            QVideoSink* sink;
            std::shared_ptr<Media::FramePipe> input;
        };

    }//Video
    struct Device
    {
        std::string name;
        std::string dsc;
    };
}//Media
