//#include "wsnetworkfactory.h"
//WSNetworkFactory::WSNetworkFactory(std::string host,int port)
//	:_handler(std::make_shared<ServerHandler>(
//		"ws://" + std::move(host) + ":" + std::to_string(port),
//		std::make_shared<rtc::WebSocket>()
//		))
//{
//}
//std::shared_ptr<AuthenticationMaster> WSNetworkFactory::createAuthenticationMaster()
//{
//	return  std::make_shared<CallerAuthenticationMaster>(_handler);
//}
//std::shared_ptr<NetworkManager> WSNetworkFactory::createNetworkManager()
//{
//	return std::make_shared<NetworkManager>(_handler);
//}
//std::shared_ptr<ControllerManager> WSNetworkFactory::createControllerManager()
//{
//	return std::make_shared<CallerControllerManager>();
//}