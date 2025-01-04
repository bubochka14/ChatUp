#include "networkmanager.h"
NetworkManager::NetworkManager(QObject* parent)
	:QObject(parent)
	,_handler(nullptr)
	,_auth(nullptr)
{
}
void NetworkManager::setCredentials(Credentials cr)
{
	if (_auth)
		_auth->setCredentials(std::move(cr));
}

bool NetworkManager::initialize()
{

	_handler = createServerHandler();
	_auth = createAuthenticationMaster();
	_active = true;
	_networkThread = std::thread(&NetworkManager::threadFunc, this);
	return true;
}
int NetworkManager::userID() const
{
	return _auth->currentUserID().value_or(User::invalidID);
}
QFuture<HashList> NetworkManager::serverMethod(const char* method,
	QVariantHash args, Priority priority)
{
	auto promise = makePromise();
	{
		std::lock_guard g(_mutex);
		_methodsQueue.emplace_back(MethodInfo{ method,std::move(args),promise, priority });
	}
	_condvar.notify_one();
	return promise->future();
}
QPromise<HashList>* NetworkManager::makePromise()
{
	return new QPromise<HashList>;
}
void NetworkManager::addClientHandler(Callback&& h, const QString& method)
{

}
void NetworkManager::threadFunc()
{
	while (1)
	{
		{
			std::unique_lock lock(_mutex);
			_condvar.wait(lock, [this]() {return !_methodsQueue.empty() || !_active; });
			if (!_active)
				return;
			std::swap(_readBuffer, _methodsQueue);
		}
		for (; !_readBuffer.empty(); _readBuffer.pop_front())
		{
			_handler->serverMethod(_readBuffer.front().method,
				std::move(_readBuffer.front().args),_readBuffer.front().prom);
		}
	}
}