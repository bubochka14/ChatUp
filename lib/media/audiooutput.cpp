#include "audiooutput.h"
using namespace Media::Audio;
Q_LOGGING_CATEGORY(LC_AUDIO_OUTPUT, "AudioOutput");

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
void Output::close()
{
	std::lock_guard g(_mutex);
	if (_isStarted)
	{
		if (_listenerIndex.has_value() && _input)
		{
			_input->removeListener(_listenerIndex.value());
			_input.reset();
			_listenerIndex = std::nullopt;
		}
		if (_sink)
			_sink->deleteLater();
		_io = nullptr;
		_isStarted = false;
	}
}
QAudioSink* Output::sink()
{
	return _sink;
}
bool Output::isStarted()
{
	return _isStarted;
}
bool Output::start(const QString& devName,std::shared_ptr<Media::FramePipe>pipe)
{
	if (isStarted())
		close();
	auto device = findDevice(devName);
	if (!device.has_value())
		return false;
	std::lock_guard g(_mutex);
	_device = device.value();
	_input = pipe;
	_listenerIndex = pipe->onDataChanged([this](std::shared_ptr<AVFrame> frame, size_t index) {
		_emp->emplaceTask([this,frame,index]() {
			std::lock_guard gm(_mutex);
			if (!_sink)
			{
				QAudioFormat format;
				//Qt и FFMPEG по разному считают семпл рейт
				format.setSampleRate(frame->sample_rate/frame->ch_layout.nb_channels);
				format.setChannelCount(frame->ch_layout.nb_channels);
				format.setSampleFormat(Media::Audio::toQtFormat((AVSampleFormat)frame->format));
				if (!_device.isFormatSupported(format)) {
					qCWarning(LC_AUDIO_OUTPUT) << "Raw audio format not supported by backend, cannot play audio.";
					_input->unmapReading(index);
					return;
				}
				_sink = new QAudioSink(_device, format);
				//connect(_sink, &QAudioSink::stateChanged, this, [=](QAudio::State st) {
				//	qCDebug(LC_AUDIO_OUTPUT) << "New audio state " << st;
				//	});
			}
			if (_sink->state() != QAudio::ActiveState)
				_io= _sink->start();
			_io->write((char*)*frame->extended_data, frame->linesize[0]);
			_input->unmapReading(index);
		});
		});
	_isStarted = true;
	return true;
}