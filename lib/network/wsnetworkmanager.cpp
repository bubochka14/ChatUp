#include "wsnetworkmanager.h"
WSNetworkManager::WSNetworkManager(const QString& host, int port, QObject* parent)
	:NetworkManager(parent)
	, ws(new WSClient(this))
{
	QUrl serverUrl("ws://" + host + ":" + QString::number(port));
	ws->setServerUrl(serverUrl);
}
AuthenticationMaster* WSNetworkManager::createAuthenticationMaster()
{
	return  new CallerAuthenticationMaster(this);;
}
ServerHandler* WSNetworkManager::createServerHandler()
{
	return new ServerHandler(ws, this);
}
