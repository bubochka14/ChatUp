#include "camerapipeline.h"
static const char unknownDevice[] = "unknown";
using namespace Media::Video;
CameraPipeline::CameraPipeline()
	:_decoder(nullptr)
	,_cam(nullptr)
	,_dev(unknownDevice)
{
}
QStringList CameraPipeline::availableDevices() const
{
	std::vector<std::string> vec = Camera::availableDevices();
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
QFuture<SourceConfig> CameraPipeline::open()
{
	if(_dev == unknownDevice)
		QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
	if (_isOpen)
		close();
	auto newCam = std::make_unique<Media::Video::Camera>(_dev.toStdString());
	auto source = newCam->open();
	if (!source.has_value())
		QtFuture::makeExceptionalFuture(std::make_exception_ptr(""));
	_cam = std::move(newCam);
	_decoder.reset(new Media::Video::Decoder(source.value()));
	_decoder->start(_cam->output());
	_isOpen = true;
	auto ret = source.value();
	return QtFuture::makeReadyValueFuture(ret);
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