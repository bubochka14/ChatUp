#pragma once
#include <mutex>
#include <optional>
#include "media.h"
#include "media_include.h"

namespace chatup {
    struct CC_MEDIA_EXPORT MediaDevice
    {
        std::string m_name;
        bool operator!=(const MediaDevice&) const;
        bool operator==(const MediaDevice&) const;
    };

	class CC_MEDIA_EXPORT MediaCaptureBase
    {
    public:
        virtual ~MediaCaptureBase() = default;
        MediaCaptureBase(const MediaCaptureBase& other) = delete;
        MediaCaptureBase(MediaCaptureBase&& other) = delete;
        MediaCaptureBase& operator=(MediaCaptureBase&& other) noexcept = delete;
        MediaCaptureBase& operator=(const MediaCaptureBase& other) = delete;

		[[nodiscard]] virtual std::optional<SourceConfig> Open(const MediaDevice& device);
        virtual void Close() = 0;

		virtual std::vector<MediaDevice> GetAvailableDevices() const = 0;
        std::optional<MediaDevice> GetOpenedDevice();
        std::shared_ptr<PacketPipe> GetOutput() const;

	protected:
        explicit MediaCaptureBase(std::shared_ptr<PacketPipe> output);
        virtual std::optional<SourceConfig> OpenInternal(const MediaDevice& device) = 0;
		std::shared_ptr<PacketPipe> m_output;
        std::optional<SourceConfig> m_lastSourceConfig;
        std::optional<MediaDevice> m_openedDevice;
    };
   
}