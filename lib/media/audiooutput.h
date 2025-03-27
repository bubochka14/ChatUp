#pragma once
#include <qaudiooutput.h>
#include "media.h"
#include <qmediadevices.h>
#include <qaudioformat.h>
#include <qaudiodevice.h>
#include <qiodevice.h>
#include <QAudioSink>
#include <QScopeGuard>
#include "core.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include "libavutil/audio_fifo.h"
}
#include <qloggingcategory.h>
#include <qiodevice.h>
#include <deque>
Q_DECLARE_LOGGING_CATEGORY(LC_AUDIO_OUTPUT)
namespace Media::Audio {
	class CC_MEDIA_EXPORT PipeAudioBuffer : public QIODevice
	{
	public:
		PipeAudioBuffer(std::shared_ptr<FramePipe> p);
	public:
		qint64 readLineData(char* data, qint64 maxSize) override
			;
		qint64 bytesAvailable() const override
			;
		qint64 readData(char* data, qint64 maxSize) override;

		/* Only readable, not writable */
		qint64 writeData(const char*, qint64) override;

		std::shared_ptr<FramePipe> pipe;
		mutable std::mutex mutex;
		size_t lastOffset = 0;
		uint64_t samples = 0;
		std::deque<std::pair<std::shared_ptr<AVFrame>, size_t>> frameDeque;
	};
	class CC_MEDIA_EXPORT Output : public QAudioOutput
	{
		Q_OBJECT;
		Q_PROPERTY(QStringList availableDevices READ availableDevices NOTIFY availableDevicesChanged);
		Q_PROPERTY(QAudioSink* sink READ sink NOTIFY sinkChanged);
	public:
		Output();
		Q_INVOKABLE bool start(const QString& dev, std::shared_ptr<Media::FramePipe>pipe);
		Q_INVOKABLE void close();
		bool isStarted();

		QStringList availableDevices();
		QAudioSink* sink();
		~Output();
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
		std::atomic<bool> _isStarted = {false};
		std::shared_ptr<Media::FramePipe> _input;
		std::optional<int> _listenerIndex;
		std::mutex _mutex;
	};
}