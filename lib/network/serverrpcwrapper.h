#pragma once
#include "network_include.h"
#include "rpc.h"
#include "data.h"
#include <taskqueue.h>
#include <qnetworkreply.h>
#include <qloggingcategory.h>
#include <rtc/rtc.hpp>
#include <unordered_map>
#include <QFuture>
Q_DECLARE_LOGGING_CATEGORY(LC_SERVER_HANDLER);
using NetworkError = QNetworkReply::NetworkError;
using JsonPromise = QPromise<json>;
class CC_NETWORK_EXPORT MethodCallFailure : std::exception
{
public:
	std::string message;
	NetworkError error;
};
class CC_NETWORK_EXPORT ServerRPCWrapper
{
public:
	explicit ServerRPCWrapper(std::string url, std::shared_ptr<rtc::WebSocket>);
	bool isConnected() const;
	QFuture<void> connect();
	void disconnect();
	void onClosed(std::function<void()> cb);
	void serverMethod(std::string, json args, std::shared_ptr<JsonPromise> output);
	using Callback = std::function<void(json&&)>;
	void addClientHandler(Callback&& h, std::string method);
protected:
	void handleConnectionError(std::string desc);
	void handleTimeout();
	void handleTextMessage(std::string);
	void handleError(std::string desc,std::shared_ptr<JsonPromise>);
	void handleMethodCall(RPC::MethodCall call);
	void handleReply(RPC::Reply reply);
private:
	std::atomic<bool> _isConnected;
	std::unordered_map<int, std::shared_ptr<JsonPromise>> _requests;
	std::unordered_map<std::string, std::vector<Callback >> _clientHandlers;
	std::shared_ptr<rtc::WebSocket> _transport;
	std::string _url;
	std::mutex _mutex;
	std::shared_ptr<QPromise<void>> _connectionPromise;
	std::optional<std::function<void()>> _closedCb;
	TaskQueue _taskQueue;

};
