#pragma once
#include "network_include.h"
#include "serverhandler.h"
#include "userhandle.h"
#include <deque>
class NetworkCoordinator;
struct CC_NETWORK_EXPORT Credentials
{
	std::string password;
	std::string login;
};
namespace Api
{
	struct Login
	{
		Login() = default;
		std::string login;
		std::string password;
		void fromCredentials(Credentials other);
		QFuture<User::Data> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "loginUser";

	};
}
class CC_NETWORK_EXPORT NetworkCoordinator : public std::enable_shared_from_this<NetworkCoordinator>
{
public:
	using Callback = std::function<void(json&&)>;
	void addClientHandler(std::string method, Callback&& h);
	void setCredentials(Credentials other);
	int currentUser() const;
	QFuture<void> initialize();

	enum Priority
	{
		AuthorizedCall,
		DirectCall
	};
	virtual QFuture<json> serverMethod(
		std::string,
		json args,
		Priority priority = AuthorizedCall);
	explicit NetworkCoordinator(std::string host, int port);
private:

	struct MethodInfo
	{
		std::string method;
		json args;
		std::unique_ptr<QPromise<json>> prom;
		int priority;
	};
	MethodInfo takeMethod();
	void threadFunc();
	Credentials _credentials;
	std::thread _networkThread;
	std::deque<MethodInfo> _authorizedCalls;
	std::deque<MethodInfo> _directCalls;
	std::atomic<int> _active;
	std::condition_variable _condvar;
	std::mutex _mutex;
	std::shared_ptr<ServerHandler> _handler;
	int _user;
};
