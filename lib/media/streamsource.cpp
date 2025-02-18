//#include "streamsource.h"
//using namespace Media;
//StreamSource::StreamSource()
//{
//
//}
//QVideoSink* StreamSource::video() const
//{
//	return _video.get();
//}
//QAudioSink* StreamSource::audio() const
//{
//	return _audio.get();
//}
//bool StreamSource::hasVideo() const
//{
//	return _video == nullptr;
//}
//bool StreamSource::hasAudio() const
//{
//	return _audio == nullptr;
//}
//void StreamSource::setVideo(std::shared_ptr<QVideoSink> sink)
//{
//	_video = sink;
//}
//void StreamSource::setAudio(std::shared_ptr<QAudioSink> sink)
//{
//	_audio = sink;
//}
//QStringList LocalStreamSource::availableAudioDevices()
//{
//	QStringList out;
//	for (auto&& i : Media::Audio::Microphone::availableDevices())
//	{
//		out.emplaceBack(QString::fromStdString(std::move(i)));
//	}
//	return out;
//}
//QStringList LocalStreamSource::availableVideoDevices()
//{
//	QStringList out;
//	for (auto&& i : Media::Video::Camera::availableDevices())
//	{
//		out.emplaceBack(QString::fromStdString(std::move(i)));
//	}
//	return out;
//}
//LocalStreamSource::LocalStreamSource()
//	:_camera(std::make_unique<Media::Video::Pipeline>())
//{
//
//}
//QFuture<void> LocalStreamSource::openVideo(const QString& device, QVideoSink* sink)
//{
//	auto source = _camera->open(device.toStdString());
//	if (!source.has_value())
//		return QtFuture::makeExceptionalFuture(std::make_exception_ptr("error"));
//	_cameraConnector.reset(new Media::Video::SinkConnector(_camera->framePipe(), sink, std::move(*source)));
//	return QtFuture::makeReadyVoidFuture();
//	
//}
//QFuture<void> LocalStreamSource::openAudio(const QString& device, QAudioSink* sink)
//{
//	_mic.reset(new Media::Audio::Microphone(device.toStdString()));
//	return QtFuture::makeReadyVoidFuture();
//}
//LocalStreamSource* LocalStreamSource::instance()
//{
//	if (!_inst)
//	{
//		Media::Init();
//		_inst = new LocalStreamSource;
//	}
//	return _inst;
//}
