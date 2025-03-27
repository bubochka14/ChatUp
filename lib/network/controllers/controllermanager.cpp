#include "controllermanager.h"
ControllerManager::ControllerManager(QObject* parent)
	:QObject(parent)
{

}
QFuture<void> ControllerManager::initializeAll()
{
	return QtFuture::makeReadyVoidFuture(); 
};
void ControllerManager::resetAll()
{}
CallerControllerManager::CallerControllerManager(std::shared_ptr<NetworkCoordinator> m, QObject* parent)
	:ControllerManager(parent)
	,message(new Message::CallerController(m,this))
	,group(new Group::CallerController(m,this))
	,call(new Call::Controller(m,this))
	,user(new User::CallerController(m,this))
{}
QFuture<void> CallerControllerManager::initializeAll()
{
	QList<QFuture<void>> futures({
		message->initialize().onFailed([this](const QString&err) 
			{_lastError = err; }),
		user->initialize().onFailed([this](const QString& err) 
			{_lastError = err; }),
		group->initialize().onFailed([this](const QString& err) 
			{_lastError = err; }),
		call->initialize().onFailed([this](const QString& err) 
			{_lastError = err; })
		});
	return QtFuture::whenAll(futures.begin(), futures.end())
		.then([this](const QList<QFuture<void>>& results){
			if (_lastError.has_value())
				throw _lastError.value();
		});

}
void CallerControllerManager::resetAll()
{
	message->reset();
	user->reset();
	group->reset();
	call->reset();
}
Group::Controller* CallerControllerManager::groupController()
{
	return group;
}
Message::Controller* CallerControllerManager::messageController()
{
	return message;
}
User::Controller* CallerControllerManager::userController()
{
	return user;
}
Call::Controller* CallerControllerManager::callController()
{
	return call;
}
