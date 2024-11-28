#pragma once
#include "wsservermethodcaller.h"
#include "clientmethoddispatcher.h"
#include "applicationsettings.h"
#include <qobject.h>
#include "callerauthentificationmaster.h"
#include "network_include.h"
#include "callerchatcontrollers.h"
#include "serverhandler.h"
class CC_NETWORK_EXPORT NetworkFactory : public  QObject
{
	Q_OBJECT;
public:
	virtual AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual ChatControllerFactory* createControllerFactory() = 0;
	virtual ServerHandler* createHandler() = 0;
	virtual ~NetworkFactory() = default;
protected:
	explicit NetworkFactory(QObject* parent = nullptr);
};
class CC_NETWORK_EXPORT WSNetworkFactory : public NetworkFactory
{
public:
	explicit WSNetworkFactory(const QString& host,int port,QObject* parent = nullptr);
	AuthenticationMaster* createAuthenticationMaster() override;
	ChatControllerFactory* createControllerFactory() override;
	ServerHandler* createHandler() override;

private:
	ClientMethodDispatcher* _disp;
	ServerMethodCaller* _caller;
	WSClient* _ws;
};