#include "networkmanager.h"
NetworkManager::NetworkManager(QObject* parent)
	:QObject(parent)
{
}

WSNetworkManager::WSNetworkManager(const QString& host, int port, QObject* parent)
	:NetworkManager(parent)
	,ws(new WSClient(QWebSocketProtocol::VersionLatest, this))
{
	QUrl serverUrl("ws://" + host + ":" + QString::number(port));
	ws->setServerUrl(serverUrl);
	_handler = new ServerHandler(ws, this);;
	_auth = new CallerAuthenticationMaster(_handler, this);
}
AuthenticationMaster* WSNetworkManager::authenticationMaster()
{
	return _auth;
}
ServerHandler* WSNetworkManager::serverHandler()
{
	return _handler;
}
