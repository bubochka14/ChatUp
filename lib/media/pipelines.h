#pragma once
#include "vector"
#include <string>
#include <qstring>
#include <memory>
#include <media.h>
#include <camerainput.h>
#include "microphoneinput.h"
#include <decoder.h>
#include "encoder.h"
#include <qfuture.h>
#include "media_include.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include "scopeguard.h"
namespace Media::Video {
    class CC_MEDIA_EXPORT CameraPipeline : public  StreamSource
    {
        Q_OBJECT;
        Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
        Q_PROPERTY(QString currentDevice READ currentDevice WRITE setCurrentDevice NOTIFY currentDeviceChanged);
    public:
        CameraPipeline();
        virtual QStringList availableDevices() const;
        std::shared_ptr<Media::FramePipe> frameOutput() override;
        QFuture<SourceConfig> open() override;
        QString currentDevice();
        void setCurrentDevice(const QString& dev);
        void close() override;
        bool isOpen() override;
    signals:
        void availableDevicesChanged();
        void currentDeviceChanged();
    protected:
        void setIsOpen(bool other);
    private:
        std::unique_ptr<Decoder> _decoder;
        std::unique_ptr<Camera>  _cam;
        std::atomic<bool> _isOpen;
        std::optional<QFuture<SourceConfig>> _openingFuture;
        std::mutex mutex;
        std::optional<SourceConfig> config;
        QString _dev;
    };
    class CC_MEDIA_EXPORT TestCameraPipeline : public CameraPipeline
    {
        Q_OBJECT;
    public:

        TestCameraPipeline();
        TestCameraPipeline(Media::Video::SourceConfig sr);
        QStringList availableDevices() const override;
        std::shared_ptr<Media::FramePipe> frameOutput() override;
        QFuture<SourceConfig> open() override;
        void close() override;
        bool isOpen() override;
    private:
        inline static QString TestDeviceName = QString("Test");
        Media::Video::SourceConfig _sourceConfig;
        std::shared_ptr<Media::FramePipe> _framePipe;

    };
}// Media::Video
namespace Media::Audio {
    class CC_MEDIA_EXPORT MicrophonePipeline : public  StreamSource
    {
        Q_OBJECT;
        Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
        Q_PROPERTY(QString currentDevice READ currentDevice WRITE setCurrentDevice NOTIFY currentDeviceChanged);
    public:
        MicrophonePipeline();
        virtual QStringList availableDevices() const;
        std::shared_ptr<Media::FramePipe> frameOutput() override;
        QFuture<SourceConfig> open() override;
        QString currentDevice();
        void setCurrentDevice(const QString& dev);
        void close() override;
        bool isOpen() override;
    signals:
        void availableDevicesChanged();
        void currentDeviceChanged();
    protected:
        void setIsOpen(bool other);
    private:
        std::unique_ptr<Decoder> _decoder;
        std::unique_ptr<Microphone>  _mic;
        std::atomic<bool> _isOpen;
        std::mutex mutex;
        std::optional<SourceConfig> config;
        std::optional<QFuture<SourceConfig>> _openingFuture;

        QString _dev;

    };
}