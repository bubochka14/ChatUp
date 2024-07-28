#pragma once
#include <QObject>
#include <QSettings>

class ApplicationSettings : public QSettings
{
	Q_OBJECT;
	Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
public:
	explicit ApplicationSettings(const QString& appName, const QString& orgName);
	QString language() const;
	QString defaultLanguage() const;
	void setLanguage( const QString& other);

signals:
	void languageChanged();
};