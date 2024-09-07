#pragma once
#include <QtPlugin>
#include "chatclient_include.h"
class CHAT_CLIENT_EXPORT StartupWindow : public QObject
{
	Q_OBJECT;
public:
	enum State
	{
		Inital,
		Loading
	};
private:
	Q_PROPERTY(float loadingProgress READ loadingProgress WRITE setLoadingProgress NOTIFY loadingProgressChanged);
	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged);
	Q_PROPERTY(QString errorString READ errorString WRITE setErrorString NOTIFY errorStringChanged)
	Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged);
public:
	State state() const;
	void setState(State other);
	virtual ~StartupWindow() = default;
	float loadingProgress() const;
	void setLoadingProgress(float other);
	QString status()const;
	void setStatus(const QString& other);
	QString errorString() const;
	void setErrorString(const QString& other);
public slots:
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void clear();
signals:
	void errorStringChanged();
	void stateChanged();
	void loadingProgressChanged();
	void statusChanged();
	void registerPassed(const QString& user, const QString& password);
	void loginPassed(const QString& user, const QString& password);
protected:
	explicit StartupWindow(QObject* parent = nullptr);
private:
	State _state;
	float _progress;
	QString _status;
	QString _error;
};