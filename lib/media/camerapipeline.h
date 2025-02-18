#pragma once
#include "vector"
#include <string>
#include <memory>
#include <media.h>
#include <camerainput.h>
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
    QStringList availableDevices() const;
    std::shared_ptr<Media::FramePipe> frameOutput() override;
    QFuture<Media::Video::SourceConfig> open() override;
    QString currentDevice();
    void setCurrentDevice(const QString& dev);
    void close() override;
    bool isOpen() override;
signals:
    void availableDevicesChanged();
    void currentDeviceChanged();
private:
    std::unique_ptr<Media::Video::Decoder> _decoder;
    std::unique_ptr<Media::Video::Camera>  _cam;
    bool _isOpen;
    std::mutex mutex;
    QString _dev;
};
