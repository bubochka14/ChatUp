#pragma once
#include "wsservermethodcaller.h"
#include "clientmethoddispatcher.h"
#include "applicationsettings.h"
#include <qmlwindowfactory.h>
#include <qobject.h>
#include "callerauthentificationmaster.h"
class ApplicationFactory : public  QObject
{
	Q_OBJECT;
public:
	virtual  ApplicationSettings* createApplicationSettings() = 0 ;
	virtual  AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual  AbstractWindowFactory* createWindowFactory(ApplicationSettings* settings) = 0;
	virtual  AbstractChatController* createChatController() = 0;
	virtual  ~ApplicationFactory() = default;
protected:
	explicit ApplicationFactory(QObject* parent = nullptr);
};
class WSApplicationFactory : public ApplicationFactory
{
public:
	explicit WSApplicationFactory(const QString& host,int port,QObject* parent);
	ApplicationSettings* createApplicationSettings();
	AuthenticationMaster* createAuthenticationMaster();
	AbstractWindowFactory* createWindowFactory(ApplicationSettings* settings);
	AbstractChatController* createChatController();
private:
	ServerMethodCaller* _caller;
	WSClient* _ws;
	QQmlEngine* _engine;
};