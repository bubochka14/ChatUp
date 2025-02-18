#pragma once
#include "network_include.h"
#include "rpc.h"
#include "data.h"
#include <taskqueue.h>
#include <qnetworkreply.h>
#include <qloggingcategory.h>
#include <rtc/rtc.hpp>
#include <unordered_map>
#include <qfuture>
Q_DECLARE_LOGGING_CATEGORY(LC_SERVER_HANDLER);
using NetworkError = QNetworkReply::NetworkError;

class CC_NETWORK_EXPORT MethodCallFailure : std::exception
{
public:
	std::string message;
	NetworkError error;
};
class CC_NETWORK_EXPORT ServerHandler
{
public:
	explicit ServerHandler(std::string url, std::shared_ptr<rtc::WebSocket>);
	bool isConnected() const;
	QFuture<void> connect();
	QFuture<json> serverMethod(std::string, json args);

	using Callback = std::function<void(json&&)>;
	void addClientHandler(Callback&& h, std::string method);
protected:
	void handleTimeout();
	void handleTextMessage(std::string);
	void handleError(std::string);
	void handleMethodCall(RPC::MethodCall call);
	void handleReply(RPC::Reply reply);
private:
	std::atomic<bool> _isConnected;
	std::unordered_map<int, std::shared_ptr<QPromise<json>>> _requests;
	std::unordered_map<std::string, std::vector<Callback >> _clientHandlers;
	std::shared_ptr<rtc::WebSocket> _transport;
	std::string _url;
	std::mutex _connectionMutex;
	std::shared_ptr<QPromise<void>> _connectionPromise;
	TaskQueue _taskQueue;

};