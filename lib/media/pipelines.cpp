#include "pipelines.h"
static const char unknownDevice[] = "unknown";
using namespace Media;
Video::CameraPipeline::CameraPipeline()
	:_decoder(nullptr)
	,_cam(nullptr)
	,_dev(unknownDevice)
{
}
QStringList Video::CameraPipeline::availableDevices() const
{
	std::vector<std::string> vec = Camera::availableDevices();
	QStringList out;
	for (auto& i : vec)
	{
		out.emplaceBack(QString::fromStdString(i));
	}return out;
}
QString Video::CameraPipeline::currentDevice()
{
	return _dev;
}
std::shared_ptr<Media::FramePipe> Video::CameraPipeline::frameOutput()
{
	return _decoder->output();
}
void Video::CameraPipeline::setCurrentDevice(const QString& dev)
{
	if (dev == _dev)
		return;
	_dev = dev;
	emit currentDeviceChanged();
}
QFuture<Video::SourceConfig> Video::CameraPipeline::open()
{
	if (!_openingFuture.has_value())
		_openingFuture = QtConcurrent::run([this]() -> Video::SourceConfig {
		scope_guard g([this]() {	_openingFuture = std::nullopt; });
			if (_dev == unknownDevice)
				throw std::string("Device is not set");
			if (_isOpen)
				close();
			if (!_cam)
			{
				_cam = std::make_unique<Camera>(_dev.toStdString());
			}
			config = _cam->open();
			if (!config.has_value())
				throw std::string("Cannot open camera");
			_decoder.reset(new Decoder(config.value()));
			_decoder->start(_cam->output());
			_isOpen = true;
			return config.value();
		});
	return _openingFuture.value();

}
void Video::CameraPipeline::close()
{
	if (_cam)
		_cam->close();
	if (_decoder)
		_decoder->stop();
	_isOpen = false;
}
bool Video::CameraPipeline::isOpen()
{
	return _isOpen;
}
void Video::CameraPipeline::setIsOpen(bool other)
{
	_isOpen = other;
}
QStringList Video::TestCameraPipeline::availableDevices() const
{
	QStringList cameraDevices = CameraPipeline::availableDevices();
	cameraDevices.push_front(TestDeviceName);
	return cameraDevices;
}
std::shared_ptr<Media::FramePipe>  Video::TestCameraPipeline::frameOutput()
{
	if (currentDevice() == TestDeviceName)
		return _framePipe;
	return CameraPipeline::frameOutput();
}
QFuture<Video::SourceConfig> Video::TestCameraPipeline::open()
{
	if (currentDevice() == TestDeviceName)
	{
		setIsOpen(true);
		return QtFuture::makeReadyFuture(_sourceConfig);
	}
	return CameraPipeline::open();
}
Video::TestCameraPipeline::TestCameraPipeline()
{
	_sourceConfig.format = AV_PIX_FMT_YUV420P;
	_sourceConfig.width = 1280;
	_sourceConfig.height = 720;
	_framePipe = Media::createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

}
Video::TestCameraPipeline::TestCameraPipeline(Video::SourceConfig sr)
	:_sourceConfig(std::move(sr))
{
	_framePipe = Media::createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

}
void Video::TestCameraPipeline::close()
{
	if (currentDevice() == TestDeviceName)
	{
		setIsOpen(false);
		return;
	}
	return CameraPipeline::close();
}
bool Video::TestCameraPipeline::isOpen()
{
	return CameraPipeline::isOpen();
}
Audio::MicrophonePipeline::MicrophonePipeline()
	:_decoder(nullptr)
	,_mic(nullptr)
	,_dev(unknownDevice)
{
}
QStringList Audio::MicrophonePipeline::availableDevices() const
{
	std::vector<std::string> vec = Microphone::availableDevices();
	QStringList out;
	for (auto& i : vec)
	{
		out.emplaceBack(QString::fromStdString(i));
	}return out;
}
QString Audio::MicrophonePipeline::currentDevice()
{
	return _dev;
}
std::shared_ptr<Media::FramePipe> Audio::MicrophonePipeline::frameOutput()
{
	return _decoder->output();
}
void Audio::MicrophonePipeline::setCurrentDevice(const QString& dev)
{
	if (dev == _dev)
		return;
	_dev = dev;
	emit currentDeviceChanged();
}
QFuture<Audio::SourceConfig> Audio::MicrophonePipeline::open()
{
	if (!_openingFuture.has_value())
		_openingFuture = QtConcurrent::run([this]() ->SourceConfig {
			scope_guard g([this]() {	_openingFuture = std::nullopt; });
			if (_dev == unknownDevice)
				throw std::string("Device is not set");
			if (_isOpen)
				close();
			auto newMic = std::make_unique<Microphone>(_dev.toStdString());
			auto source = newMic->open();
			if (!source.has_value())
				throw std::string("Cannot open microphone");
			_mic = std::move(newMic);
			_decoder.reset(new Decoder(source.value()));
			_decoder->start(_mic->output());
			_isOpen = true;
			return source.value();
		});
	return _openingFuture.value();

}
void Audio::MicrophonePipeline::close()
{
	if (_mic)
		_mic->close();
	if (_decoder)
		_decoder->stop();
	_isOpen = false;
}
bool Audio::MicrophonePipeline::isOpen()
{
	return _isOpen;
}
void Audio::MicrophonePipeline::setIsOpen(bool other)
{
	_isOpen = other;
}