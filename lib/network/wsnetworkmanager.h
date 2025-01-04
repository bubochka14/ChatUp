#pragma once
#include "network_include.h"
#include "networkmanager.h"
#include "callerauthentificationmaster.h"
class CC_NETWORK_EXPORT WSNetworkManager : public NetworkManager
{
public:
	explicit WSNetworkManager(const QString& host, int port, QObject* parent = nullptr);
protected:
	AuthenticationMaster* createAuthenticationMaster() override;
	ServerHandler* createServerHandler() override;
private:
	WSClient* ws;
};