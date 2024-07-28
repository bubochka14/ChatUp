#include "applicationsettings.h"
ApplicationSettings::ApplicationSettings(const QString& appName, const QString& orgName)
	:QSettings(appName, orgName)
{
	

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
	setValue("language", other);
}