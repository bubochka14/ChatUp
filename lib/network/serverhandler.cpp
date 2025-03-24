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
		handleConnectionError(ex.what());
	}
	return _connectionPromise->future();

}
void ServerHandler::handleConnectionError(std::string desc)
{
	if (!_connectionPromise)
		return;
	qCWarning(LC_SERVER_HANDLER) << "Connection error: " << desc;
	MethodCallFailure out;
	out.message = desc;
	out.error = QNetworkReply::ConnectionRefusedError;
	_connectionPromise->setException(std::make_exception_ptr(out));
	_connectionPromise.reset();
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
			_requests[reply.repTo]->setException(std::make_exception_ptr(reply.error.value_or("")));

		else if (reply.status == RPC::Success)
		{
			_requests[reply.repTo]->addResult(std::move(reply.reply));
			_requests[reply.repTo]->finish();
		}
		else
			_requests[reply.repTo]->setException(std::make_exception_ptr(std::string("Unknown error")));

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
void ServerHandler::onClosed(std::function<void()> cb)
{
	_closedCb = std::move(cb);
}

ServerHandler::ServerHandler(std::string url, std::shared_ptr<rtc::WebSocket> transport)
	:_isConnected(false)
	,_url(std::move(url))
	,_transport(transport)
{
	_transport->onError([this](std::string err) {
		_taskQueue.enqueue([this, err = std::move(err)]() {
			handleConnectionError(std::move(err));
			});
		});
	_transport->onClosed([this]() {
		if (!_isConnected)
			return;
		_isConnected = false;
		if (_closedCb.has_value())
			_closedCb.value()();
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
}
void ServerHandler::handleError(std::string desc, std::shared_ptr<JsonPromise> prom)
{
	MethodCallFailure out;
	out.message = std::move(desc);
	out.error = QNetworkReply::InternalServerError;
	prom->setException(std::make_exception_ptr(out));
	prom.reset();
}

void ServerHandler::serverMethod(std::string method, json args, std::shared_ptr<JsonPromise> output)
{
	output->start();
	QFuture<json> outFuture = output->future();
	RPC::MethodCall outMsg = RPC::MessageConstructor::methodCallMsg(
		std::move(method), std::move(args)
	);
	int messageID = outMsg.id;
	_requests.emplace(messageID, std::move(output));
	try {
		_transport->send(json(std::move(outMsg)).dump());
	}
	catch (std::exception ex)
	{
		handleError(ex.what(),output);
	}
}
void ServerHandler::addClientHandler(Callback&& h, std::string method)
{
	_clientHandlers[method].emplace_back(std::move(h));

}
