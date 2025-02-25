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
class CC_MEDIA_EXPORT CameraPipeline  : public  Media::Video::StreamSource
{
    Q_OBJECT;
    Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
    Q_PROPERTY(QString currentDevice READ currentDevice WRITE setCurrentDevice NOTIFY currentDeviceChanged);
public:
    CameraPipeline();
    virtual QStringList availableDevices() const;
    std::shared_ptr<Media::FramePipe> frameOutput() override;
    QFuture<Media::Video::SourceConfig> open() override;
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
    std::unique_ptr<Media::Video::Decoder> _decoder;
    std::unique_ptr<Media::Video::Camera>  _cam;
    std::atomic<bool> _isOpen;
    std::mutex mutex;
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
    QFuture<Media::Video::SourceConfig> open() override;
    void close() override;
    bool isOpen() override;
private:
    inline static QString TestDeviceName = QString("Test");
    Media::Video::SourceConfig _sourceConfig;
    std::shared_ptr<Media::FramePipe> _framePipe;
};
class CC_MEDIA_EXPORT MicrophonePipeline : public  Media::Audio::StreamSource
{
    Q_OBJECT;
    Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
    Q_PROPERTY(QString currentDevice READ currentDevice WRITE setCurrentDevice NOTIFY currentDeviceChanged);
public:
    MicrophonePipeline();
    virtual QStringList availableDevices() const;
    std::shared_ptr<Media::FramePipe> frameOutput() override;
    QFuture<Media::Audio::SourceConfig> open() override;
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
    std::unique_ptr<Media::Audio::Decoder> _decoder;
    std::unique_ptr<Media::Audio::Microphone>  _mic;
    std::atomic<bool> _isOpen;
    std::mutex mutex;
    QString _dev;

};