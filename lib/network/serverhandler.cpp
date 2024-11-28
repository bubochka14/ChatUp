#include "serverhandler.h"
bool ServerHandler::isConnected() const
{
	return isCon;
}
void ServerHandler::setIsConnected(bool other)
{
	if (other == isCon)
		return;
	isCon = other;
	emit isConnectedChanged();
}
ServerHandler::ServerHandler(QObject* parent)
	:QObject(parent)
	,isCon(false)
{

}
WSServerHandler::WSServerHandler(WSClient* cl)
{
	connect(cl, &WSClient::closed, this, [this]() {setIsConnected(false); });
	connect(cl, &WSClient::connected, this, [this]() {setIsConnected(true); });
	setIsConnected(cl->isConnected());
}
