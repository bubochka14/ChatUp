#include "usercontroller.h"
using namespace User;
Handler::Handler(User::Data&& data, QObject* parent)
	:QObject(parent)
	, _data(std::move(data))
{

}
void Handler::release()
{

}
QString Handler::name() const
{
	return _data.name;
}
QString Handler::tag() const
{
	return _data.tag;
}
int Handler::id() const
{
	return _data.id;
}
User::Status Handler::status() const
{
	return _data.status;
}
CallerController::CallerController(NetworkManager* manager,
	QObject* parent)
	:Controller(parent)
	,_manager(manager)
{

}
Controller::Controller(QObject* parent)
	:AbstractController(parent)
{

}
Handler* CallerController::currentUser()
{
	if (_manager->userID() == User::invalidID)
		return _empty;
	if (!_userHandlers.contains(_manager->userID()))
		return _empty;
	return _userHandlers[_manager->userID()];
}
QFuture<Model*> CallerController::find(const QVariantHash& pattern, int limit)
{
	return QtFuture::makeReadyFuture<Model*>(new Model);
}
QFuture<Handler*>CallerController::get(int userID)
{
	if (_userHandlers.contains(userID))
		return QtFuture::makeReadyValueFuture(_userHandlers[userID]);
	if (_pendingRequests.contains(userID))
		return _pendingRequests[userID];
	Api::Get req;
	req.id = userID;
	_pendingRequests[userID] = req.exec(_manager)
		.then([this,userID](User::Data&& res) {
		auto handler = new Handler(std::move(res),this);
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
QFuture<void> CallerController::initialize()
{
	if (_manager->userID() == User::invalidID)
		return QtFuture::makeExceptionalFuture(std::make_exception_ptr(QString("Unauthorized")));
	return get(_manager->userID()).then([](Handler*) {

		});
}