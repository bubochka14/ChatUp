#include "applicationsettings.h"
ApplicationSettings::ApplicationSettings()
	:QSettings("bubochka","ChatClient")
{}
//void ApplicationSettings::initialize(const QString& appName, const QString& orgName)
//{
//	_settings()
//}
ApplicationSettings* ApplicationSettings::create(QQmlEngine* qmlEngine, QJSEngine*)
{
	return instance();
}
ApplicationSettings* ApplicationSettings::instance()
{
	if (!_inst)
		_inst = new ApplicationSettings();
	return _inst;
}
QString ApplicationSettings::language() const
{
	return value("language", defaultLanguage()).toString();
}
QString ApplicationSettings::defaultLanguage() const
{
	return QString("en");
}
void ApplicationSettings::setLanguage(const QString& other)
{
	QString last = language();
	setValue("language", other);
	if (other != last)
		emit languageChanged();
}
QString ApplicationSettings::videoDevice() const
{
	return value("videoDevice", defaultVideoDevice()).toString();
}
QString ApplicationSettings::defaultVideoDevice() const
{
	return QString("unknown");
}
void ApplicationSettings::setVideoDevice(const QString& other)
{
	QString last = videoDevice();
	setValue("videoDevice", other);
	if (other != last)
		emit videoDeviceChanged();
}
QString ApplicationSettings::audioDevice() const
{
	return value("audioDevice", defaultAudioDevice()).toString();
}
QString ApplicationSettings::defaultAudioDevice() const
{
	return QString("unknown");
}
void ApplicationSettings::setAudioDevice(const QString& other)
{
	QString last = audioDevice();
	setValue("audioDevice", other);
	if (other != last)
		emit audioDeviceChanged();
}