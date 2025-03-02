#include "audiooutput.h"
using namespace Media::Audio;
Output::Output()
	:emp(new QtEventLoopEmplacer)
{
}
bool Output::start(std::shared_ptr<Media::FramePipe>pipe)
{
	QAudioFormat format;
	device = QMediaDevices::defaultAudioOutput();
	format.setSampleRate(24000);
	format.setChannelCount(2);
	format.setSampleFormat(Media::Audio::toQtFormat((AVSampleFormat)8));
	if (!device.isFormatSupported(format)) {
		qWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return false;
	}
	QAudioSink* sink = new QAudioSink(device, format);
	connect(sink, &QAudioSink::stateChanged, this, [=](QAudio::State st) {
		qDebug() << "audio state " << st;


		});
	sink->start();
	pipe->onDataChanged([this, sink, wPipe = std::weak_ptr(pipe)](std::shared_ptr<AVFrame> frame, size_t index) {
		emp->emplaceTask([=]() {
			if (sink->state() != QAudio::ActiveState)
				dev = sink->start();
			dev->write((char*)*frame->extended_data, frame->linesize[0]);
			if (auto pipe = wPipe.lock())
				pipe->unmapReading(index);
			});
		});

	return true;
}