#pragma once
#include "wsservermethodcaller.h"
#include "clientmethoddispatcher.h"
#include "applicationsettings.h"
#include <qobject.h>
#include "callerauthentificationmaster.h"
#include "chatclient_include.h"
#include "callerchatcontrollers.h"
class CHAT_CLIENT_EXPORT NetworkFactory : public  QObject
{
	Q_OBJECT;
public:
	virtual AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual ChatControllerFactory* createControllerFactory() = 0;
	virtual ~NetworkFactory() = default;
protected:
	explicit NetworkFactory(QObject* parent = nullptr);
};
class CHAT_CLIENT_EXPORT WSNetworkFactory : public NetworkFactory
{
public:
	explicit WSNetworkFactory(const QString& host,int port,QObject* parent = nullptr);
	AuthenticationMaster* createAuthenticationMaster() override;
	ChatControllerFactory* createControllerFactory() override;

private:
	ClientMethodDispatcher* _disp;
	ServerMethodCaller* _caller;
	WSClient* _ws;
};