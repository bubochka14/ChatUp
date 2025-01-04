#include "serverhandler.h"
using namespace Qt::Literals::StringLiterals;
Q_LOGGING_CATEGORY(LC_SERVER_HANDLER, "ServerHandler");
bool ServerHandler::isConnected() const
{
	return _isConnected;
}
void ServerHandler::setIsConnected(bool other)
{
	if (other == _isConnected)
		return;
	_isConnected = other;
}
void ServerHandler::handleTextMessage(std::string msg)
{
	QJsonObject&& obj = QJsonDocument::fromJson(msg.toUtf8()).object();
	if (obj.value("type") == "response")
	{
		WSReply* reply = new WSReply;
		if (!reply->extractFromHash(obj.toVariantHash()))
		{
			qCWarning(LC_SERVER_HANDLER) << "Cannot handle reply message";
			//reply->deleteLater();
		}
		else
		{
			handleReply(reply);
			//if(autoDeleteMessages())
			//    reply->deleteLater();
		}
	}
	else if (obj.value("type") == "methodCall")
	{
		WSMethodCall* call = new WSMethodCall;
		qDebug() << obj.toVariantHash();
		if (!call->extractFromHash(obj.toVariantHash()))
		{
			qCWarning(LC_SERVER_HANDLER) << "Cannot handle methodCall message";
			//call->deleteLater();
		}
		else
		{
			emit handleMethodCall(call);
			//if (autoDeleteMessages())
			//    call->deleteLater();
		}
  }
  else
  {
      qCWarning(LC_SERVER_HANDLER) << "Unknown message type received";
  }
}
ServerHandler::ServerHandler(Transport* transport,QObject* parent)
	:QObject(parent)
	,_isConnected(false)
	,_transport(transport)
{
	_transport->connect2Server();

	connect(_transport, &Transport::errorReceived, this, &ServerHandler::handleError/*, Qt::DirectConnection*/);
	connect(_transport, &Transport::textReceived, this,&ServerHandler::handleTextMessage/*, Qt::DirectConnection*/);
	//_timeoutTimer.start(5000);
}
void ServerHandler::handleError(const QString& err)
{
	for (auto& req : _requests)
	{
		MethodCallFailure out;
		out.message = err;
		out.error = QNetworkReply::UnknownNetworkError;
		req->setException(std::make_exception_ptr(out));
		delete req;
	}
	_requests.clear();
}

void ServerHandler::serverMethod(const char* method, QVariantHash args, QPromise<HashList>* prom)
{
	WSMethodCall* outMsg = WSMessageConstructor::methodCallMsg(method, std::move(args));
	_requests.insert(outMsg->messageId(), prom);
	_transport->sendText(QJsonDocument::fromVariant(outMsg->toHash()).toJson());
	
	_requests[outMsg->messageId()]->start();
}
void ServerHandler::addClientHandler(Callback&& h, const QString& method)
{
	_clientHandlers[method].emplaceBack(std::move(h));
}

void ServerHandler::handleReply(WSReply* wsrep)
{
	if (!_requests.contains(wsrep->replyTo()))
		return;
	if (wsrep->status() == WSReply::error)
	{
		MethodCallFailure out;
		out.message = wsrep->errorString();
		out.error = QNetworkReply::InternalServerError;
		_requests[wsrep->replyTo()]->setException(std::make_exception_ptr(wsrep->errorString()));
	}
	else if (wsrep->status() == WSReply::success)
	{
		_requests[wsrep->replyTo()]->addResult(wsrep->reply());
		_requests[wsrep->replyTo()]->finish();
	}
	else
	{
		MethodCallFailure* out = new MethodCallFailure;
		out->message = "Unknown methodCall status"_L1;
		_requests[wsrep->replyTo()]->setException(std::make_exception_ptr(out));
	}
	delete _requests[wsrep->replyTo()];
	_requests.remove(wsrep->replyTo());
	wsrep->deleteLater();

}
void ServerHandler::handleTimeout()
{

}
void ServerHandler::handleMethodCall(WSMethodCall* call)
{
	if (_clientHandlers.contains(call->method()))
	{
		for (auto& i : _clientHandlers[call->method()])
		{
			i(call->args());
		}
	}
}
