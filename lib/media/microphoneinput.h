#pragma once
extern "C"
{
#include <libavdevice/avdevice.h>
}
#include <string>
#include <memory>
#include "media.h"
#include <optional>
#include "media_include.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_MICROPHONE);

namespace Media::Audio 
{
    class CC_MEDIA_EXPORT Microphone
    {
    public:
        explicit Microphone(std::string devcice);
        std::shared_ptr<PacketPipe> output();
        std::optional<Audio::SourceConfig> open();
        bool isOpened();
        void close();
        static std::vector<std::string> availableDevices();
        std::string currentDevice() const;
        ~Microphone();
        std::shared_ptr<AVFormatContext> context();
    private:
        std::shared_ptr<AVFormatContext> ctx;
        std::shared_ptr<PacketPipe> out;
        std::string device;
        std::thread micThread;
        void threadFunc();
        std::mutex mutex;
        std::atomic<bool> active;
    };
}