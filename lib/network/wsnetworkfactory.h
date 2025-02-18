//#pragma once
//#include "network_include.h"
//#include "networkfactory.h"
//#include "callerauthentificationmaster.h"
//#include <rtc/websocket.hpp>
//
//class CC_NETWORK_EXPORT WSNetworkFactory final : public NetworkFactory
//{
//public:
//	explicit WSNetworkFactory(std::string host, int port);
//protected:
//	std::shared_ptr<AuthenticationMaster> createAuthenticationMaster() override;
//	std::shared_ptr<NetworkManager>   createNetworkManager() override;
//	std::shared_ptr<ControllerManager> createControllerManager() override;
//
//private:
//	std::shared_ptr<ServerHandler>  _handler;
//};