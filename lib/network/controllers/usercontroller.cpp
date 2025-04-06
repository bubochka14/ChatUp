#include "usercontroller.h"
using namespace User;
CallerController::CallerController(std::shared_ptr<NetworkCoordinator> manager,QObject* parent)
	:Controller(parent)
	,_manager(manager)
{
	User::Data dumbUser;
	dumbUser.name = "null";
	dumbUser.tag = "null";
	_empty = new User::Handle(this);
	_empty->extractFromData(std::move(dumbUser));
	growHandlePool(256);
	Api::Update::handle(_manager,[this](Api::Update::UserUpdate upd) {
		if (!_userHandlers.contains(upd.id))
			return;
		if (upd.name.has_value())
			_userHandlers[upd.id]->setName(QString::fromStdString(std::move(*upd.name)));
		if (upd.tag.has_value())
			_userHandlers[upd.id]->setTag(QString::fromStdString(std::move(*upd.name)));
		if (upd.status.has_value())
			_userHandlers[upd.id]->setStatus(*upd.status);
		});
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
		req.tag = "^"+pattern.right(pattern.size()-1).toStdString();
	else
		req.name = "^" + pattern.toStdString();
	return true;
}
Handle* CallerController::getFreeHandle()
{
	if(!_freeHandlePool.size())
		growHandlePool(std::max((size_t)64, _userHandlers.size()));
	auto handle = _freeHandlePool.top();
	_freeHandlePool.pop();
	return handle;
}
QFuture<Model*> CallerController::getGroupUsers(int groupID)
{
	std::lock_guard g(_handleMutex);
	if (_usersInRooms.contains(groupID))
		return QtFuture::makeReadyValueFuture(_usersInRooms[groupID]);
	Group::Api::GetUsers req;
	req.roomID = groupID;
	return req.exec(_manager).then([this,groupID](std::vector<Data>&& res) {
		auto out = new Model;
		_usersInRooms[groupID] = out;
		std::vector<User::Handle*> handleList;
		for (auto& user : res)
		{
			if (!_userHandlers.contains(user.id))
			{
				auto newHandle = getFreeHandle();
				newHandle->extractFromData(std::move(user));
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
				auto newHandle = getFreeHandle();
				newHandle->extractFromData(std::move(user));
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
	return Api::Get().exec(_manager).then([this](User::Data&& res) {
		std::lock_guard g(_handleMutex);
		auto handler = getFreeHandle();
		handler->extractFromData(std::move(res));
		return handler;
	});
}
QFuture<Handle*>CallerController::get(int userID)
{
	std::lock_guard g(_handleMutex);
	if (_userHandlers.contains(userID))
		return QtFuture::makeReadyValueFuture(_userHandlers[userID]);
	if (_pendingRequests.contains(userID))
		return _pendingRequests[userID];
	Api::Get req;
	req.id = userID;
	_pendingRequests[userID] = req.exec(_manager).then([this,userID](User::Data&& res) {
		std::lock_guard g(_handleMutex);
		auto handle = getFreeHandle();
		handle->extractFromData(std::move(res));
		_userHandlers.emplace(userID, handle);
		_pendingRequests.remove(userID);
		return handle;
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
void CallerController::growHandlePool(size_t size)
{
	//run on controller`s thread
	auto future = QtFuture::makeReadyVoidFuture().then(this,[this, size]() {
		std::lock_guard g(_handleMutex);
		for (size_t i = 0; i < size; i++)
		{
			_freeHandlePool.emplace(new Handle(this));
		}
	});
	future.waitForFinished();
}