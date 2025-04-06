#pragma once
#include "network_include.h"
#include "serverrpcwrapper.h"
#include "userhandle.h"
#include <deque>
#include "data.h"
class NetworkCoordinator;
struct CC_NETWORK_EXPORT Credentials
{
	std::string login;
	std::string password;
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
	int  currentUser() const;
	void setReconnectionCount(int other);
	void disconnect();
	bool isActive();
	void onDisconnected(std::function<void()> cb);
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
	~NetworkCoordinator();
private:

	struct MethodInfo
	{
		std::string method;
		json args;
		std::shared_ptr<QPromise<json>> prom;
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
	mutable std::mutex _mutex ;
	std::shared_ptr<ServerRPCWrapper> _rpc;
	std::optional<std::function<void()>> _disconnectedCb;
	int _reconnectionCount;
	int _user;
};
