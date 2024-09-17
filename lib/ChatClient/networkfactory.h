#pragma once
#include "wsservermethodcaller.h"
#include "clientmethoddispatcher.h"
#include "applicationsettings.h"
#include <qobject.h>
#include "callerauthentificationmaster.h"
#include "chatclient_include.h"
#include "chatcontroller.h"

class CHAT_CLIENT_EXPORT NetworkFactory : public  QObject
{
	Q_OBJECT;
public:
	virtual AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual AbstractChatController* createChatController() = 0;
	virtual ClientMethodDispatcher* createDispatcher() = 0;
	virtual ~NetworkFactory() = default;
protected:
	explicit NetworkFactory(QObject* parent = nullptr);
};
class CHAT_CLIENT_EXPORT WSNetworkFactory : public NetworkFactory
{
public:
	explicit WSNetworkFactory(const QString& host,int port,QObject* parent = nullptr);
	AuthenticationMaster* createAuthenticationMaster() override;
	AbstractChatController* createChatController() override;
	ClientMethodDispatcher* createDispatcher() override;

private:
	ServerMethodCaller* _caller;
	WSClient* _ws;
};