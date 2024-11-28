#pragma once
#include <QObject>
#include <QSettings>
#include "core_include.h"

class CC_CORE_EXPORT ApplicationSettings : public QSettings
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