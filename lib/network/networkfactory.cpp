#include "networkfactory.h"
NetworkFactory::NetworkFactory(QObject* parent)
	:QObject(parent)
{
}

WSNetworkFactory::WSNetworkFactory(const QString& host,int port,QObject* parent)
	:NetworkFactory(parent)
	,_ws(new WSClient(QWebSocketProtocol::VersionLatest))
	
{
	_disp = new WSClientMethodDispatcher(_ws);
	_caller = new WSServerMethodCaller(_ws,host, port);
}
AuthenticationMaster* WSNetworkFactory::createAuthenticationMaster()
{
	return new CallerAuthentificationMaster(_caller,parent());
}
ControllerManager* WSNetworkFactory::createControllerManager()
{
	return new CallerControllerManager(_caller,_disp);
}
ServerHandler* WSNetworkFactory::createHandler()
{
	return new WSServerHandler(_ws);
}