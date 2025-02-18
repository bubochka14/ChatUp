#include "usercontroller.h"
using namespace User;
CallerController::CallerController(std::shared_ptr<NetworkCoordinator> manager,
	QObject* parent)
	:Controller(parent)
	,_manager(manager)
{
	User::Data dumpUser;
	dumpUser.name = "null";
	dumpUser.tag = "null";
	_empty = new User::Handle(std::move(dumpUser), this);
}
Controller::Controller(QObject* parent)
	:AbstractController(parent)
{

}
Handle* Controller::empty() const
{
	return getEmpty();
}
Handle* CallerController::getEmpty() const
{
	return _empty;
}

QFuture<Model*> CallerController::find(const QVariantHash& pattern, int limit)
{
	return QtFuture::makeReadyFuture<Model*>(new Model);
}
QFuture<Handle*> CallerController::get()
{
	return Api::Get().exec(_manager).then(this,[this](User::Data&& res) {

		auto handler = new Handle(std::move(res),this);
		return handler;
		});
}
QFuture<Handle*>CallerController::get(int userID)
{
	if (_userHandlers.contains(userID))
		return QtFuture::makeReadyValueFuture(_userHandlers[userID]);
	if (_pendingRequests.contains(userID))
		return _pendingRequests[userID];
	Api::Get req;
	req.id = userID;
	_pendingRequests[userID] = req.exec(_manager)
		.then(this,[this,userID](User::Data&& res) 
			{
				auto handler = new Handle(std::move(res),this);
				_userHandlers.emplace(userID, handler);
				return handler;
			//_pendingRequests.remove(userID);
		});
	return _pendingRequests[userID];
}
QFuture<void> CallerController::update(const QVariantHash& data)
{
	return QtFuture::makeReadyVoidFuture();
}
QFuture<void> CallerController::remove()
{
	return QtFuture::makeReadyVoidFuture();

}
QFuture<void> CallerController::create(const QString& password, const QString& log)
{
	User::Api::Create req;
	req.login = log.toStdString();
	req.password = password.toStdString();
	return req.exec(_manager);
}
QFuture<void> CallerController::initialize()
{
	return QtFuture::makeReadyVoidFuture();
}