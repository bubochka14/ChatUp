#pragma once

#include <QObject>
#include <QDebug>
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
namespace media {
    
    class CC_MEDIA_EXPORT CameraInput
    {
    public:
        struct Config
        {

            std::string name;
            std::string dsc;

        };
        explicit CameraInput(Config info);
        std::shared_ptr<media::PacketPipe> output() ;
        std::optional<Video::Source> open() ;
        bool isOpened();
        void close() ;
        static std::vector<Config> availableDevices();
        Config currentDevice() const;
        ~CameraInput();
        AVFormatContext* ctx();
    private:
        void threadFunc();
        std::shared_ptr<media::PacketPipe> out;
        AVFormatContext* ictx;
        std::mutex mutex;
        std::thread cameraThread;
        Config device;
        std::atomic<bool> active;
    };
}