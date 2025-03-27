#include "usercontroller.h"
using namespace User;
CallerController::CallerController(std::shared_ptr<NetworkCoordinator> manager,QObject* parent)
	:Controller(parent)
	,_manager(manager)
{
	User::Data dumbUser;
	dumbUser.name = "null";
	dumbUser.tag = "null";
	_empty = new User::Handle(std::move(dumbUser), this);
	Group::Api::AddUser::handle(_manager, [this](Group::Api::AddUser::Desc req) {
		if (_usersInRooms.contains(req.roomID))
		{
			Model* model = _usersInRooms[req.roomID];
			if (_userHandlers.contains(req.userID))
				model->insertHandler(_userHandlers[req.userID]);
			else
			{
				get(req.userID).then([model](User::Handle* res) {
					model->insertHandler(res);
			});
			}
		}
	});
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
bool CallerController::parseSearchString(const QString& pattern, Api::Find& req)
{
	if(!pattern.size())
		return false;
	if (pattern[0] == "@")
		req.tag = "^"+pattern.right(1).toStdString();
	else
		req.name = "^" + pattern.toStdString();
	return true;
}
QFuture<Model*> CallerController::getGroupUsers(int groupID)
{
	if (_usersInRooms.contains(groupID))
		return QtFuture::makeReadyValueFuture(_usersInRooms[groupID]);
	Group::Api::GetUsers req;
	req.roomID = groupID;
	return req.exec(_manager).then(this,[this,groupID](std::vector<Data>&& res) {
		auto out = new Model;
		_usersInRooms[groupID] = out;
		std::vector<User::Handle*> handleList;
		for (auto& user : res)
		{
			if (!_userHandlers.contains(user.id))
			{
				auto newHandle = new User::Handle(std::move(user), this);
				_userHandlers.emplace(user.id, newHandle);
				handleList.push_back(newHandle);
			}
			else
				handleList.push_back(_userHandlers[user.id]);
		}
		out->reset(handleList);
		return out;
	});
}
QFuture<Model*> CallerController::find(const QString& pattern, int limit)
{
	Api::Find req;
	parseSearchString(pattern, req);
	req.limit = limit;
	req.useRegexp = true;
	return req.exec(_manager).then(this,[this](std::vector<User::Data>&& res) {
		//qml ownership
		Model* out = new Model;
		std::vector<User::Handle*> _handles;
		for (auto& user : res)
		{
			if (_userHandlers.contains(user.id))
				_handles.push_back(_userHandlers[user.id]);
			else
			{
				auto newHandle = new User::Handle(std::move(user), this);
				_userHandlers[newHandle->id()] = newHandle;
				_handles.push_back(newHandle);
			}
		}
		out->reset(_handles);
		return out;
	});
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
	_pendingRequests[userID] = req.exec(_manager).then(this,[this,userID](User::Data&& res) {
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
void CallerController::reset()
{
	_userHandlers.clear();
	_usersInRooms.clear();
}