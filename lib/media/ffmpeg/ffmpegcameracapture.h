#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "MediaCaptureBase.h"

extern "C" {
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libswscale/swscale.h>
}

namespace chatup
{
    class CC_MEDIA_EXPORT FFmpegCameraCapture final : public MediaCaptureBase
    {
    public:
        using io_context = boost::asio::io_context;
        explicit FFmpegCameraCapture(std::shared_ptr<io_context> context);
        ~FFmpegCameraCapture() override;

    	void Close() override;
        std::vector<MediaDevice> GetAvailableDevices() const override;
    private:
        std::optional<SourceConfig> OpenInternal(const MediaDevice& device) override;

        void ReadFrameSpin();
        std::shared_ptr<io_context> m_asioContext;
        std::unique_ptr<AVFormatContext> m_formatContext;
        boost::thread workerThread;
        std::atomic<bool> m_active;
    };
}