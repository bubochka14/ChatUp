#include "clientmethoddispatcher.h"
Q_LOGGING_CATEGORY(LC_WS_DISPATCHER,"WSClientMethodDispatcher")
ClientMethodDispatcher::ClientMethodDispatcher(QObject* parent)
	:QObject(parent)
{
}
WSClientMethodDispatcher::WSClientMethodDispatcher(WSClient* client, QObject* parent)
	:ClientMethodDispatcher(parent)
	,_ws(client)
{
	connect(_ws, &WSClient::methodCallReceived, this, &WSClientMethodDispatcher::handleMethodCall);
	_handlers["updateUser"] = QList<Handler>() <<[=](const QVariantHash& hash) {emit updatedUser(hash); };
	_handlers["updateRoom"] = QList<Handler>() <<[=](const QVariantHash& hash) {emit updatedRoom(hash); };
	_handlers["deleteMessage"]= QList<Handler>() << [=](const QVariantHash& hash) {emit deletedMessage(hash); };
	_handlers["deleteRoom"] = QList<Handler>() << [=](const QVariantHash& hash) {emit deletedRoom(hash); };
	_handlers["disconnect"]= QList<Handler>() << [=](const QVariantHash& hash) {emit disconnected(hash); };
	_handlers["updateMessage"] = QList<Handler>() << [=](const QVariantHash& hash) {emit updatedMessage(hash); };
	_handlers["postMessage"]= QList<Handler>() << [=](const QVariantHash& hash) {emit messagePosted(hash); };
}
void WSClientMethodDispatcher::handleMethodCall(WSMethodCall* call)
{
	if (_handlers.contains(call->method()))
	{
		for (auto& handler : _handlers[call->method()])
		{
			handler(call->args());
		}
	}
}
void WSClientMethodDispatcher::addCustomHandler(const QString& method, std::function<void(const QVariantHash&)> handler)
{
	if (_handlers.contains(method))
		_handlers[method].append(handler);
	else _handlers[method] = QList<std::function<void(const QVariantHash&)>>() << handler;
}