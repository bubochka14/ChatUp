#pragma once
#include <qaudiooutput.h>
#include "media.h"
#include <qmediadevices.h>
#include <qaudioformat.h>
#include <qaudiodevice.h>
#include <qiodevice.h>
#include <QAudioSink>
#include "core.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libavutil/audio_fifo.h"

}
namespace Media::Audio {
	class CC_MEDIA_EXPORT Output : public QAudioOutput
	{
		Q_OBJECT;
		Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
		Q_PROPERTY(QAudioSink* sink READ sink NOTIFY sinkChanged);
	public:
		Output();
		Q_INVOKABLE bool start(const QString& dev, std::shared_ptr<Media::FramePipe>pipe);
		QStringList availableDevices();
		QAudioSink* sink();
	signals:
		void availableDevicesChanged();
		void sinkChanged();
	private:
		void setSink(QAudioSink* sink);
		std::optional<QAudioDevice> findDevice(const QString& str);
		QIODevice* _io;
		QAudioDevice _device;
		QtEventLoopEmplacer* _emp;
		QAudioSink* _sink;
	};
}