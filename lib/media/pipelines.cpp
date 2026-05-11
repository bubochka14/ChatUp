#include "pipelines.h"
static const char unknownDevice[] = "unknown";
using namespace chatup;
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
std::shared_ptr<FramePipe> CameraPipeline::frameOutput()
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
	if (!_openingFuture.has_value())
		_openingFuture = QtConcurrent::run([this]() -> SourceConfig {
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
			_decoder->open(_cam->output());
			_isOpen = true;
			return config.value();
		});
	return _openingFuture.value();

}
void CameraPipeline::close()
{
	if (_cam)
		_cam->close();
	if (_decoder)
		_decoder->close();
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
std::shared_ptr<FramePipe>  TestCameraPipeline::frameOutput()
{
	if (currentDevice() == TestDeviceName)
		return _framePipe;
	return CameraPipeline::frameOutput();
}
QFuture<SourceConfig> TestCameraPipeline::open()
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
	_framePipe = createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

}
TestCameraPipeline::TestCameraPipeline(SourceConfig sr)
	:_sourceConfig(std::move(sr))
{
	_framePipe = createFramePipe(_sourceConfig.width, _sourceConfig.height, _sourceConfig.format);

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
Audio::MicrophonePipeline::MicrophonePipeline()
	:_decoder(nullptr)
	,_mic(nullptr)
	,_dev(unknownDevice)
	, _out (createFramePipe())

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
std::shared_ptr<FramePipe> Audio::MicrophonePipeline::frameOutput()
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
void Audio::MicrophonePipeline::setFilterFactory(std::shared_ptr<FilterFactory> other)
{
	_filterFactory = other;
}
QFuture<Audio::SourceConfig> Audio::MicrophonePipeline::open()
{
	if (!_openingFuture.has_value())
		_openingFuture = QtConcurrent::run([this]() ->SourceConfig {
			scope_guard g([this]() {_openingFuture = std::nullopt; });
			if (_dev == unknownDevice)
			{
				auto devList = availableDevices();
				if(devList.isEmpty())
					throw std::string("No audio devices");
				_dev = devList.first();
			} 

			if (_isOpen)
				close();
			auto newMic = std::make_unique<Microphone>(_dev.toStdString());
			auto source = newMic->open();
			if (!source.has_value())
				throw std::string("Can not open the microphone");
			_mic = std::move(newMic);
			_decoder.reset(new Decoder(source.value()));
			_decoder->open(_mic->output());
			if (_filterFactory)
				_filter = _filterFactory->createFilter(source.value());
			if (_filter)
			{
				auto outConfig = _filter->open(_decoder->output());
				if(!outConfig.has_value())
					throw std::string("Cannot open the filter");
				_filter->output()->onDataChanged([this](auto frame, size_t index) {
					auto data = _out->holdForWriting();
					data.ptr = frame;
					_filter->output()->unmapReading(index);
					_out->unmapWriting(data.subpipe, true);
			
				});
				_isOpen = true;
				return outConfig.value();
			}
			//else
			//{
			//	_decoder->output()->onDataChanged([this](auto frame, size_t index) {
			//		FramePipe::SubpipeHandle data = _out->holdForWriting();
			//		_out->setStoredData(index, frame);
			//		_out->unmapWriting(data.subpipe, true);
			//		_decoder->output()->unmapReading(index);

			//		});
			//	_isOpen = true;
			//	return source.value();
			//}
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
		_decoder->close();
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