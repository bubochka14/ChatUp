#include "pipelines.h"
static const char unknownDevice[] = "unknown";
using namespace Media;
CameraPipeline::CameraPipeline()
	:_decoder(nullptr)
	,_cam(nullptr)
	,_dev(unknownDevice)
{
}
QStringList CameraPipeline::availableDevices() const
{
	std::vector<std::string> vec = Video::Camera::availableDevices();
	QStringList out;
	for (auto& i : vec)
	{
		out.emplaceBack(QString::fromStdString(i));
	}return out;
}
QString CameraPipeline::currentDevice()
{
	return _dev;
}
std::shared_ptr<Media::FramePipe> CameraPipeline::frameOutput()
{
	return _decoder->output();
}
void CameraPipeline::setCurrentDevice(const QString& dev)
{
	if (dev == _dev)
		return;
	_dev = dev;
	emit currentDeviceChanged();
}
QFuture<Video::SourceConfig> CameraPipeline::open()
{
	if(_dev == unknownDevice)
		QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
	if (_isOpen)
		close();
	if(!_cam)
	{
		_cam = std::make_unique<Media::Video::Camera>(_dev.toStdString());
	}
	config = _cam->open();
	if (!config.has_value())
		return QtFuture::makeExceptionalFuture<Video::SourceConfig>(std::make_exception_ptr(""));
	_decoder.reset(new Media::Video::Decoder(config.value()));
	_decoder->start(_cam->output());
	_isOpen = true;
	return QtFuture::makeReadyValueFuture(config.value());
}
void CameraPipeline::close()
{
	if (_cam)
		_cam->close();
	if (_decoder)
		_decoder->stop();
	_isOpen = false;
}
bool CameraPipeline::isOpen()
{
	return _isOpen;
}
void CameraPipeline::setIsOpen(bool other)
{
	_isOpen = other;
}
QStringList TestCameraPipeline::availableDevices() const
{
	QStringList cameraDevices = CameraPipeline::availableDevices();
	cameraDevices.push_front(TestDeviceName);
	return cameraDevices;
}
std::shared_ptr<Media::FramePipe>  TestCameraPipeline::frameOutput()
{
	if (currentDevice() == TestDeviceName)
		return _framePipe;
	return CameraPipeline::frameOutput();
}
QFuture<Media::Video::SourceConfig> TestCameraPipeline::open()
{
	if (currentDevice() == TestDeviceName)
	{
		setIsOpen(true);
		return QtFuture::makeReadyFuture(_sourceConfig);
	}
	return CameraPipeline::open();
}
TestCameraPipeline::TestCameraPipeline()	
{
	_sourceConfig.format = AV_PIX_FMT_YUV420P;
	_sourceConfig.width = 1280;
	_sourceConfig.height = 720;
	_framePipe = Media::createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

}
TestCameraPipeline::TestCameraPipeline(Media::Video::SourceConfig sr)
	:_sourceConfig(std::move(sr))
{
	_framePipe = Media::createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

}
void TestCameraPipeline::close()
{
	if (currentDevice() == TestDeviceName)
	{
		setIsOpen(false);
		return;
	}
	return CameraPipeline::close();
}
bool TestCameraPipeline::isOpen()
{
	return CameraPipeline::isOpen();
}
MicrophonePipeline::MicrophonePipeline()
	:_decoder(nullptr)
	,_mic(nullptr)
	,_dev(unknownDevice)
{
}
QStringList MicrophonePipeline::availableDevices() const
{
	std::vector<std::string> vec = Media::Audio::Microphone::availableDevices();
	QStringList out;
	for (auto& i : vec)
	{
		out.emplaceBack(QString::fromStdString(i));
	}return out;
}
QString MicrophonePipeline::currentDevice()
{
	return _dev;
}
std::shared_ptr<Media::FramePipe> MicrophonePipeline::frameOutput()
{
	return _decoder->output();
}
void MicrophonePipeline::setCurrentDevice(const QString& dev)
{
	if (dev == _dev)
		return;
	_dev = dev;
	emit currentDeviceChanged();
}
QFuture<Audio::SourceConfig> MicrophonePipeline::open()
{
	if (_dev == unknownDevice)
		QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
	if (_isOpen)
		close();
	auto newMic = std::make_unique<Media::Audio::Microphone>(_dev.toStdString());
	auto source = newMic->open();
	if (!source.has_value())
		return QtFuture::makeExceptionalFuture<Audio::SourceConfig>(std::make_exception_ptr(""));
	_mic = std::move(newMic);
	_decoder.reset(new Media::Audio::Decoder(source.value()));
	_decoder->start(_mic->output());
	_isOpen = true;
	auto ret = source.value();
	return QtFuture::makeReadyValueFuture(ret);
}
void MicrophonePipeline::close()
{
	if (_mic)
		_mic->close();
	if (_decoder)
		_decoder->stop();
	_isOpen = false;
}
bool MicrophonePipeline::isOpen()
{
	return _isOpen;
}
void MicrophonePipeline::setIsOpen(bool other)
{
	_isOpen = other;
}