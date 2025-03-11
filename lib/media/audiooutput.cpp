#include "audiooutput.h"
using namespace Media::Audio;
Output::Output()
	:_emp(new QtEventLoopEmplacer)
	,_sink(nullptr)
	,_io(nullptr)
{
}
QStringList Output::availableDevices()
{
	QStringList out;
	for (auto& i : QMediaDevices::audioOutputs())
	{
		out.emplaceBack(std::move(i.description()));
	}return out;
}
std::optional<QAudioDevice> Output::findDevice(const QString& dev)
{
	for (auto& i : QMediaDevices::audioOutputs())
	{
		if (i.description() == dev)
			return i;
	}
	return std::nullopt;
}
void Output::setSink(QAudioSink* sink)
{
	if (_sink == sink)
		return;
	_sink = sink;
	sinkChanged();
}
QAudioSink* Output::sink()
{
	return _sink;
}
bool Output::start(const QString& devName,std::shared_ptr<Media::FramePipe>pipe)
{
	QAudioFormat format;
	auto device = findDevice(devName);
	if (!device.has_value())
		return false;
	_device = std::move(*device);
	format.setSampleRate(24000);
	format.setChannelCount(2);
	format.setSampleFormat(Media::Audio::toQtFormat((AVSampleFormat)8));
	if (!_device.isFormatSupported(format)) {
		qWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return false;
	}
	if(_sink)
		_sink = new QAudioSink(_device, format);
	connect(_sink, &QAudioSink::stateChanged, this, [=](QAudio::State st) {
		qDebug() << "audio state " << st;


		});
	_sink->start();
	pipe->onDataChanged([this, wPipe = std::weak_ptr(pipe)](std::shared_ptr<AVFrame> frame, size_t index) {
		_emp->emplaceTask([=]() {
			if (_sink->state() != QAudio::ActiveState)
				_io= _sink->start();
			_io->write((char*)*frame->extended_data, frame->linesize[0]);
			if (auto pipe = wPipe.lock())
				pipe->unmapReading(index);
			});
		});

	return true;
}