#pragma once
#include "applicationsettings.h"
#include <qobject.h>
#include "callerauthentificationmaster.h"
#include "network_include.h"
#include "serverhandler.h"

class CC_NETWORK_EXPORT NetworkManager : public  QObject
{
	Q_OBJECT;
public:
	virtual AuthenticationMaster* authenticationMaster() =0;
	virtual ServerHandler* serverHandler() =0;
protected:
	explicit NetworkManager(QObject* parent = nullptr);
};
class CC_NETWORK_EXPORT WSNetworkManager : public NetworkManager
{
public:
	explicit WSNetworkManager(const QString& host, int port, QObject * parent = nullptr);
	AuthenticationMaster* authenticationMaster() override;
	ServerHandler* serverHandler() override;
private:
	AuthenticationMaster* _auth;
	ServerHandler* _handler;
	WSClient* ws;
};
