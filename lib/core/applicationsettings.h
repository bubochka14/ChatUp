#pragma once
#include <QObject>
#include <QSettings>
#include "core_include.h"
#include <qqmlengine.h>
class CC_CORE_EXPORT ApplicationSettings : public QSettings
{
	Q_OBJECT;
	QML_SINGLETON;
	QML_ELEMENT;
	Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
	Q_PROPERTY(QString videoDevice READ videoDevice WRITE setVideoDevice NOTIFY videoDeviceChanged);
	Q_PROPERTY(QString audioDevice READ audioDevice WRITE setAudioDevice NOTIFY audioDeviceChanged);
public:
	//static void initialize(const QString& appName, const QString& orgName);
	static ApplicationSettings* instance();
	static ApplicationSettings* create(QQmlEngine* qmlEngine, QJSEngine*);
	QString videoDevice() const;
	QString defaultVideoDevice() const;
	void setVideoDevice( const QString& other);

	QString language() const;
	QString defaultLanguage() const;
	void setLanguage(const QString& other);

	QString audioDevice() const;
	QString defaultAudioDevice() const;
	void setAudioDevice(const QString& other);

signals:
	void languageChanged();
	void videoDeviceChanged();
	void audioDeviceChanged();
protected:
	ApplicationSettings();
private:
	inline static ApplicationSettings* _inst = nullptr;
	QSettings _settings;
};