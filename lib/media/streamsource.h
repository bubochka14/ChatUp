//#pragma once
//#include "media_include.h"
//#include <qobject.h>
//#include <qvideosink.h>
//#include <qfuture>
//#include <qaudiosink.h>
//#include "microphoneinput.h"
//#include "camerapipeline.h"
//namespace Media {
//
//	class CC_MEDIA_EXPORT LocalStreamSource final: public StreamSource
//	{
//		Q_OBJECT;
//		Q_PROPERTY(QStringList availableAudioDevices READ availableAudioDevices NOTIFY availableAudioDevicesChanged);
//		Q_PROPERTY(QStringList availableVideoDevices READ availableVideoDevices NOTIFY availableVideoDevicesChanged);
//	public:
//		static LocalStreamSource* instance();
//		QStringList availableVideoDevices();
//		QStringList availableAudioDevices();
//
//		Q_INVOKABLE QFuture<void> openVideo(const QString& device, QVideoSink* sink);
//		Q_INVOKABLE QFuture<void> openAudio(const QString& device, QAudioSink* sink);
//	signals:
//		void availableVideoDevicesChanged();
//		void availableAudioDevicesChanged();
//
//	private:
//		LocalStreamSource();
//		std::unique_ptr<Media::Video::Pipeline> _camera;
//		std::unique_ptr<Media::Audio::Microphone> _mic;
//		std::unique_ptr<Media::Video::SinkConnector> _cameraConnector;
//	};
//}