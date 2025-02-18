//#include "callernetworkmanager.h"
//CallerNetworkManager::CallerNetworkManager(std::shared_ptr<ServerHandler> handler)
//	:_handler(handler)
//{
//}
//
//QFuture<void> CallerNetworkManager::initialize()
//{
//	//add reinit checking 
//	return _handler->connect().then([this]() {
//		_networkThread = std::thread(&CallerNetworkManager::threadFunc, this);
//		});
//}
//QFuture<json> CallerNetworkManager::serverMethod(std::string method,
//	json args, Priority priority)
//{
//	auto promise = std::make_unique<QPromise<json>>();
//	auto future = promise->future();
//	{
//		std::lock_guard g(_mutex);
//		_methodsQueue.emplace_back(MethodInfo{
//			std::move(method),
//			std::move(args),
//			std::move(promise),
//			priority
//			});
//	}
//	_condvar.notify_one();
//	return future;
//}
//
//void CallerNetworkManager::addMethodHandler(Callback&& h, std::string method)
//{
//
//}
//void CallerNetworkManager::threadFunc()
//{
//	while (1)
//	{
//		{
//			std::unique_lock lock(_mutex);
//			_condvar.wait(lock, [this]() {return !_methodsQueue.empty() && _active; });
//			std::swap(_readBuffer, _methodsQueue);
//		}
//		for (; !_readBuffer.empty(); _readBuffer.pop_front())
//		{
//			_handler->serverMethod(_readBuffer.front().method,
//				std::move(_readBuffer.front().args)).then(
//					[promise = std::move(_readBuffer.front().prom)](json&& res) {
//						promise->emplaceResult(std::move(res));
//					});
//		}
//	}
//}