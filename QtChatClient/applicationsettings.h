#pragma once
#include <QObject>
#include <QSettings>

class ApplicationSettings : public QSettings
{
	Q_OBJECT;
	Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
public:
	// APP_NAME, ORG_NAME - CMake definitions
	explicit ApplicationSettings(const QString& appName = APP_NAME, const QString& orgName = ORG_NAME);
	QString language() const;
	QString defaultLanguage() const;
	void setLanguage( const QString& other);

signals:
	void languageChanged();
};