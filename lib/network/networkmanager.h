#pragma once
#include "applicationsettings.h"
#include <qobject.h>
#include "authenticationmaster.h"
#include "network_include.h"
#include "serverhandler.h"
#include <deque>
class CC_NETWORK_EXPORT NetworkManager : public  QObject
{
	Q_OBJECT;
public:
	using Callback = std::function<void(QVariantHash&&)>;
	void addClientHandler(Callback&& h, const QString& method);
	int userID() const;
	void setCredentials(Credentials cr);
	bool initialize();
	enum Priority
	{
		QueuedCall,
		TopQueuedCall,
		DirectCall
	};
protected:
	struct MethodInfo
	{
		const char* method;
		QVariantHash args;
		QPromise<HashList>* prom;
		int priority;
	};
public:
	QFuture<HashList> serverMethod(const char* method, QVariantHash args, Priority priority= QueuedCall);
protected:
	explicit NetworkManager(QObject* parent = nullptr);
	virtual AuthenticationMaster* createAuthenticationMaster() = 0;
	virtual ServerHandler* createServerHandler() = 0;
	QPromise<HashList>* makePromise();
private:
	void threadFunc();
	std::thread _networkThread;
	std::deque<MethodInfo> _methodsQueue;
	std::deque<MethodInfo> _readBuffer;
	std::atomic<int> _active;
	std::condition_variable _condvar;
	std::mutex _mutex;
	ServerHandler* _handler;
	AuthenticationMaster* _auth;
};
