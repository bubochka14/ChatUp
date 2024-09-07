#pragma once
#include <QObject>
#include <QSettings>
#include "chatclient_include.h"

class CHAT_CLIENT_EXPORT ApplicationSettings : public QSettings
{
	Q_OBJECT;
	Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
public:
	explicit ApplicationSettings(const QString& appName, const QString& orgName, QObject*parent = nullptr);
	QString language() const;
	QString defaultLanguage() const;
	void setLanguage( const QString& other);

signals:
	void languageChanged();
};