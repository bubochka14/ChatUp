#include "networkfactory.h"
NetworkFactory::NetworkFactory(QObject* parent)
	:QObject(parent)
{
}

WSNetworkFactory::WSNetworkFactory(const QString& host,int port,QObject* parent)
	:NetworkFactory(parent)
	,_ws(new WSClient(QWebSocketProtocol::VersionLatest))
	
{
	_caller = new WSServerMethodCaller(_ws,host, port);
}
AuthenticationMaster* WSNetworkFactory::createAuthenticationMaster()
{
	return new CallerAuthentificationMaster(_caller,parent());
}
AbstractChatController* WSNetworkFactory::createChatController()
{
	return new CallerChatController(_caller);
}
ClientMethodDispatcher* WSNetworkFactory::createDispatcher()
{
	return new WSClientMethodDispatcher(_ws);
}
