#include "networkmanager.h"
using namespace std::chrono_literals;

void Api::Login::fromCredentials(Credentials other)
{
	login = std::move(other.login);
	password = std::move(other.password);
}

QFuture<User::Data> Api::Login::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, {
		{"login",login },{"password",password} }, NetworkCoordinator::DirectCall)
		.then([](json&& res) {
		return User::Data(res);
			});
}
int NetworkCoordinator::currentUser() const
{
	return _user;
}
NetworkCoordinator::NetworkCoordinator(std::string host, int port)
	:_handler(std::make_shared<ServerHandler>(
		"ws://" + std::move(host) + ":" + std::to_string(port),
		std::make_shared<rtc::WebSocket>()))
	,_user(User::invalidID)
	,_reconnectionCount(5)
{
	_active = true;
	_networkThread = std::thread(&NetworkCoordinator::threadFunc, this);

}
QFuture<void> NetworkCoordinator::initialize()
{
	Api::Login req;
	req.fromCredentials(_credentials);
	return req.exec(shared_from_this()).then([this](User::Data&& res){
			_user = res.id;
			_condvar.notify_one();
		});							
}
void NetworkCoordinator::setCredentials(Credentials other)
{
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
void NetworkCoordinator::setReconnectionCount(int other)
{
	_reconnectionCount = other;
}
void NetworkCoordinator::addClientHandler(std::string method, Callback&& h)
{
	_handler->addClientHandler(std::move(h), method);
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
					&& _active;
			});
			for(int i=0;i< _reconnectionCount &&!_handler->isConnected();i++)
			{
				try
				{
					_handler->connect().waitForFinished();
				}
				catch (...) {}
				if (!_handler->isConnected())
					std::this_thread::sleep_for(200ms);
			}
			info = takeMethod();
			if (!_handler->isConnected())
			{
				info.prom->setException(std::make_exception_ptr(std::string("Server is not responding")));
				info.prom.reset();
				continue;
			}
		}
		_handler->serverMethod(info.method, std::move(info.args), std::move(info.prom));
	}
}