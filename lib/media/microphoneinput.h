#pragma once
extern "C"
{
#include <libavdevice/avdevice.h>
}
#include <string>
#include <memory>
#include "media.h"
#include <optional>
#include <qdebug.h>
#include "media_include.h"
namespace media {
    class CC_MEDIA_EXPORT MicrophoneInput
    {
    public:
        struct Device
        {

            std::string name;
            std::string dsc;

        };
        explicit MicrophoneInput(const Device& info);
        std::shared_ptr<PacketPipe> output();
        std::optional<Audio::Source> open();
        bool isOpened();
        void close();
        static std::vector<Device> availableDevices();
        Device currentDevice() const;
        ~MicrophoneInput();
        std::shared_ptr<AVFormatContext> context();
    private:
        std::shared_ptr<AVFormatContext> ctx;
        std::shared_ptr<PacketPipe> out;
        Device device;
        std::thread micThread;
        void threadFunc();
        std::mutex mutex;
        std::atomic<bool> active;
    };
}