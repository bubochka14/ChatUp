#include "networkcoordinator.h"
using namespace std::chrono_literals;
void NetworkCoordinator::disconnect()
{
	_rpc->disconnect();
	{
		std::lock_guard g(_mutex);
		_user = User::invalidID;
	}
}
void Api::Login::fromCredentials(Credentials other)
{
	login = std::move(other.login);
	password = std::move(other.password);
}

QFuture<User::Data> Api::Login::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, 
		{ {"login",login },{"password",password} }, NetworkCoordinator::DirectCall)
		.then([](json&& res) {
			return User::Data(res);
		});
}
int NetworkCoordinator::currentUser() const
{
	std::lock_guard g(_mutex);
	return _user;
}
NetworkCoordinator::NetworkCoordinator(std::string host, int port)
	:_user(User::invalidID)
	,_reconnectionCount(5)
{
	rtc::WebSocketConfiguration conf;
	conf.disableTlsVerification = true;
	_rpc = std::make_shared<ServerRPCWrapper>(
		"wss://" + std::move(host) + ":" + std::to_string(port),
		std::make_shared<rtc::WebSocket>(std::move(conf)));
	_rpc->onClosed([this]() {
		std::lock_guard g(_mutex);
		if (_disconnectedCb.has_value())
			_disconnectedCb.value()();
	});
	_active = true;
	_networkThread = std::thread(&NetworkCoordinator::threadFunc, this);

}
QFuture<void> NetworkCoordinator::initialize()
{
	Api::Login req;
	req.fromCredentials(_credentials);
	return req.exec(shared_from_this()).then([this](User::Data&& res){
		std::lock_guard g(_mutex);
		_user = res.id;
		_condvar.notify_one();
	});							
}
void NetworkCoordinator::onDisconnected(std::function<void()> cb)
{
	std::lock_guard g(_mutex);
	_disconnectedCb = std::move(cb);
}
void NetworkCoordinator::setCredentials(Credentials other)
{
	std::lock_guard g(_mutex);
	_credentials =  std::move(other);
}
QFuture<json> NetworkCoordinator::serverMethod(std::string method,
	json args, Priority priority)
{
	auto promise = std::make_unique<QPromise<json>>();
	promise->start();
	auto future = promise->future();
	switch (priority) {
		case DirectCall:
			{
				std::lock_guard g(_mutex);
				_directCalls.emplace_back(MethodInfo{
					std::move(method),
					std::move(args),
					std::move(promise),
					priority
				});
			}
			break;
		case AuthorizedCall:
		{
			std::lock_guard g(_mutex);
			_authorizedCalls.emplace_front(MethodInfo{
				std::move(method),
				std::move(args),
				std::move(promise),
				priority
				});
			break;

		}
	}
	_condvar.notify_one();
	
	return future;
}
NetworkCoordinator::~NetworkCoordinator()
{
	disconnect();
	_active = false;
	_condvar.notify_all();
	if (_networkThread.joinable())
		_networkThread.join();
}
void NetworkCoordinator::setReconnectionCount(int other)
{
	std::lock_guard g(_mutex);
	_reconnectionCount = other;
}
void NetworkCoordinator::addClientHandler(std::string method, Callback&& h)
{
	_rpc->addClientHandler(std::move(h), method);
}
NetworkCoordinator::MethodInfo NetworkCoordinator::takeMethod()
{
	if (!_directCalls.empty())
	{
		auto res = std::move(_directCalls.front());
		_directCalls.pop_front();
		return res;
	}
	if (!_authorizedCalls.empty())
	{
		auto res = std::move(_authorizedCalls.front());
		_authorizedCalls.pop_front();
		return res;
	}

}
void NetworkCoordinator::threadFunc()
{
	MethodInfo&& info = MethodInfo();
	while (1)
	{
		{
			std::unique_lock lock(_mutex);
			_condvar.wait(lock, [this]() {
				return (!_directCalls.empty()
					|| !_authorizedCalls.empty())
					|| !_active;
			});
			if (!_active)
				return;
			for(int i=0;i< _reconnectionCount &&!_rpc->isConnected();i++)
			{
				try
				{
					_rpc->connect().waitForFinished();
				}
				catch (...) {}
				if (!_rpc->isConnected())
					std::this_thread::sleep_for(200ms);
			}
			info = takeMethod();
			if (!_rpc->isConnected())
			{
				info.prom->setException(std::make_exception_ptr(std::string("Server is not responding")));
				info.prom.reset();
				continue;
			}
		}
		_rpc->serverMethod(info.method, std::move(info.args), std::move(info.prom));
	}
}
bool NetworkCoordinator::isActive()
{
	return _active;
}