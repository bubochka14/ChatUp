 #pragma once

#include <QObject>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavdevice/avdevice.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}
#include <mutex>
#include "media.h"
#include "media_include.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_CAMERA)

namespace Media::Video {
    class CC_MEDIA_EXPORT Camera
    {
    public:
        explicit Camera(std::string device);
        std::shared_ptr<Media::PacketPipe> output() ;
        std::optional<Video::SourceConfig> open() ;
        bool isOpened();
        void close();
        static std::vector<std::string> availableDevices();
        Device currentDevice() const;
        ~Camera();
        AVFormatContext* ctx();
    private:
        void threadFunc();
        std::shared_ptr<Media::PacketPipe> out;
        AVFormatContext* ictx;
        std::mutex mutex;
        std::thread cameraThread;
        std::string device;
        std::atomic<bool> active;
    };
}