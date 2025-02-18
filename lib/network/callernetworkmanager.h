//#pragma once
//#include "network_include.h"
//#include "serverhandler.h"
//#include "networkmanager.h"
//#include <deque>
//class CC_NETWORK_EXPORT CallerNetworkManager final : public NetworkManager
//{
//public:
//	using Callback = std::function<void(json&&)>;
//	void addMethodHandler(Callback&& h, std::string method) override;
//	QFuture<void> initialize() override;
//	virtual QFuture<json> serverMethod(
//		std::string name,
//		json args,
//		Priority priority = QueuedCall) override;
//	explicit CallerNetworkManager(std::shared_ptr<ServerHandler> handler);
//
//protected:
//	struct MethodInfo
//	{
//		std::string method;
//		json args;
//		std::unique_ptr<QPromise<json>> prom;
//		int priority;
//	};
//private:
//	void threadFunc();
//	std::thread _networkThread;
//	std::deque<MethodInfo> _methodsQueue;
//	std::deque<MethodInfo> _readBuffer;
//	std::atomic<int> _active;
//	std::condition_variable _condvar;
//	std::mutex _mutex;
//	std::shared_ptr<ServerHandler> _handler;
//};
