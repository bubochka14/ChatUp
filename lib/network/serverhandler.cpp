#include "serverhandler.h"
using namespace Qt::Literals::StringLiterals;
Q_LOGGING_CATEGORY(LC_SERVER_HANDLER, "ServerHandler");
bool ServerHandler::isConnected() const
{
	return _isConnected;
}
QFuture<void> ServerHandler::connect()
{
	{
		std::lock_guard g(_connectionMutex);
		if (!_connectionPromise)
		{
			_connectionPromise = std::make_shared<QPromise<void>>();
			_connectionPromise->start();
		}
		else return _connectionPromise->future();
	}
	try {
		qCDebug(LC_SERVER_HANDLER) << "Connection to " << _url;
		_transport->open(_url);
	}
	catch (std::exception ex)
	{
		qCWarning(LC_SERVER_HANDLER) << "Connection error: " << ex.what();
		handleError(ex.what());
		_connectionPromise->setException(std::make_exception_ptr(ex.what()));
	}
	return _connectionPromise->future();

}
void ServerHandler::handleTextMessage(std::string msg)
{
	auto parsed = json::parse(std::move(msg));
	if (parsed.value("type","") == "response")
	{
		RPC::Reply reply = json::parse(msg);
		if (!_requests.contains(reply.repTo))
			return;
		if (reply.status == RPC::Error)
		{
			MethodCallFailure out;
			out.message = std::move(reply.error.value_or(""));
			out.error = QNetworkReply::InternalServerError;
			_requests[reply.repTo]->setException(std::make_exception_ptr(out));
		}
		else if (reply.status == RPC::Success)
		{
			_requests[reply.repTo]->addResult(std::move(reply.reply));
			_requests[reply.repTo]->finish();
		}
		else
		{
			MethodCallFailure out;
			out.message = "Unknown methodCall status";
			_requests[reply.repTo]->setException(std::make_exception_ptr(out));
		}
		_requests[reply.repTo].reset();
		_requests.erase(reply.repTo);
	}
	else if (parsed.value("type", "") == "methodCall")
	{
		RPC::MethodCall call = json::parse(msg);
		qCDebug(LC_SERVER_HANDLER) << "Client call " << call.method << msg;
		if (_clientHandlers.contains(call.method))
		{
			for (auto& i : _clientHandlers[call.method])
			{
				i(std::move(call.args));
			}
		}
  }
  else
  {
      qCWarning(LC_SERVER_HANDLER) << "Unknown message type received";
  }
}
ServerHandler::ServerHandler(std::string url, std::shared_ptr<rtc::WebSocket> transport)
	:_isConnected(false)
	,_url(std::move(url))
	,_transport(transport)
{
	_transport->onError([this](std::string err) {
		_taskQueue.enqueue([this, err = std::move(err)]() {
			handleError(std::move(err));
			});
		});
	_transport->onMessage([this](auto msg) {
		if (!std::holds_alternative<std::string>(msg))
			return;
		handleTextMessage((std::get<std::string>(msg)));

		});
	_transport->onOpen([this]() {
		_taskQueue.enqueue([this]() {
			qCDebug(LC_SERVER_HANDLER) << "server open";
			std::lock_guard g(_connectionMutex);
			if (_connectionPromise)
			{
				_isConnected = true;
				_connectionPromise->finish();
				_connectionPromise = nullptr;
			}
			else
				qCCritical(LC_SERVER_HANDLER) << "Error: server open, but no connection request";
			});
		});
	//_transport->connect2Server();
	//_timeoutTimer.start(5000);
}
void ServerHandler::handleError(std::string err)
{
	for (auto& req : _requests)
	{
		MethodCallFailure out;
		out.message = err;
		out.error = QNetworkReply::UnknownNetworkError;
		req.second->setException(std::make_exception_ptr(out));
	}
	_connectionPromise->setException(std::make_exception_ptr("connectionError"));
	_connectionPromise.reset();
	_requests.clear();
}

QFuture<json> ServerHandler::serverMethod(std::string method, json args)
{
	auto promise = std::make_shared<QPromise<json>>();
	promise->start();
	QFuture<json> outFuture = promise->future();
	RPC::MethodCall outMsg = RPC::MessageConstructor::methodCallMsg(
		std::move(method), std::move(args)
	);
	int messageID = outMsg.id;
	_requests.emplace(messageID, std::move(promise));
	try {
		_transport->send(json(std::move(outMsg)).dump());
	}
	catch (std::exception ex)
	{
		handleError(ex.what());
	}
	return outFuture;
}
void ServerHandler::addClientHandler(Callback&& h, std::string method)
{
	_clientHandlers[method].emplace_back(std::move(h));

}
