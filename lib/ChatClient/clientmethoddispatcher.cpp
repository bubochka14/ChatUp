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
	_handlers["disconnect"]= QList<Handler>() << [=](const QVariantHash& hash) {emit disconnected(hash); };

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
void WSClientMethodDispatcher::handler(const QString& method, std::function<void(const QVariantHash&)> handler)
{
	if (_handlers.contains(method))
		_handlers[method].append(handler);
	else _handlers[method] = QList<std::function<void(const QVariantHash&)>>() << handler;
}