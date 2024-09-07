#pragma once
#include "wsservermethodcaller.h"
#include "clientmethoddispatcher.h"
#include "applicationsettings.h"
#include <qmlwindowfactory.h>
#include <qobject.h>
#include "callerauthentificationmaster.h"
#include "chatclient_include.h"

class CHAT_CLIENT_EXPORT ApplicationFactory : public  QObject
{
	Q_OBJECT;
public:
	virtual ApplicationSettings* createApplicationSettings() = 0 ;
	virtual AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual AbstractWindowFactory* createWindowFactory(ApplicationSettings* settings) = 0;
	virtual AbstractChatController* createChatController() = 0;
	virtual ClientMethodDispatcher* createDispatcher() = 0;
	virtual ~ApplicationFactory() = default;
protected:
	explicit ApplicationFactory(QObject* parent = nullptr);
};
class CHAT_CLIENT_EXPORT WSApplicationFactory : public ApplicationFactory
{
public:
	explicit WSApplicationFactory(const QString& host,int port,QObject* parent);
	ApplicationSettings* createApplicationSettings() override;
	AuthenticationMaster* createAuthenticationMaster() override;
	AbstractWindowFactory* createWindowFactory(ApplicationSettings* settings) override;
	AbstractChatController* createChatController() override;
	ClientMethodDispatcher* createDispatcher() override;

private:
	ServerMethodCaller* _caller;
	WSClient* _ws;
	QQmlEngine* _engine;
};