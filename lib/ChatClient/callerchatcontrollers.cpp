#include "callerchatcontrollers.h"
CallerRoomController::CallerRoomController(ServerMethodCaller* caller, QObject* parent)
	:RoomController(parent)
	, _caller(caller)
	, _groups(new RoomModel(this))
{

}
CallerMessageController::CallerMessageController(ServerMethodCaller* caller, QObject* parent)
	:MessageController(parent)
	, _caller(caller)
	, _tempMessageCounter(0)

{

}
CallerUserController::CallerUserController(ServerMethodCaller* caller, QObject* parent)
	:UserController(parent)
	,_caller(caller)
{

}


QFuture<void> CallerRoomController::createGroup(const QString& name) 
{
	return _caller->createRoom({ {"name",name},{"type","group"} }).then(this, [=](QVariantHash&& hash) {
		_groups->insertRow(0);
		_groups->setData(_groups->index(0), hash, RoomModel::HashRole);
		});
}
QFuture<void> CallerRoomController::createDirect(int userID) 
{
	//todo
	return QtFuture::makeReadyFuture();
}

QFuture<RoomModel*> CallerRoomController::getGroups()
{
	return _caller->getUserRooms({ {"userId",currentUser()->id()},{"type","group"}})
	.then([=](HashList&& rooms)
		{
			_groups->insertRows(0, rooms.count());
			size_t roomsCount = rooms.count();
			for (size_t i = 0; i < roomsCount; i++)
			{
				_groups->setData(_groups->index(0+ i), rooms.takeFirst(), RoomModel::HashRole);
			}
			return _groups;
		});
}
//QFuture<QAbstractListModel*> getDirectRooms() {}
QFuture<void> CallerRoomController::addUserToRoom(int userID, int roomID)
{
	return _caller->addUserToRoom({ {"roomId",roomID}, {"userId",userID} }).then([](QVariantHash&&){});
}

QFuture<void> CallerRoomController::deleteRoom(int id) 
{
	return QtFuture::makeReadyFuture();

	//return _caller->deleteRoom(id).then([=](QVariantHash&& hash) {
	//	auto&& index = _userRooms->idToIndex(id);
	//	if(index.isValid())
	//	_userRooms->removeRow(index.row());
	//	});
}
QFuture<void> CallerRoomController::updateRoom(const QVariantHash& data)
{
	//todo
	return QtFuture::makeReadyFuture();
}

QFuture <MessageModel*> CallerMessageController::getRoomHistory(int roomId)
{
	if (_history.contains(roomId))
		return QtFuture::makeReadyFuture(_history.value(roomId));
	return _caller->getRoomHistory({ { "roomId", roomId } }).then(this, [=](HashList&& messages) {
		_history.insert(roomId, new MessageModel(this));
		auto model = _history[roomId];
		model->insertRows(0, messages.count());
		for (size_t i = 0; i < messages.count(); i++)
		{
			model->setData(model->index(i), messages[i], MessageModel::HashRole);

		}
		return model;
		});
}
QFuture<void> CallerMessageController::updateMessage(const QVariantHash& data)
{
	QVariantHash out(data);
	return _caller->updateMessage(std::move(out)).then(this, [=](QVariantHash&& hash) {
		auto model = _history[data["roomId"].toUInt()];
		auto&& index = model->idToIndex(data["id"].toInt());
		if(index.isValid())
			model->setData(index, hash, MessageModel::HashRole);
		});
}
QFuture<void> CallerMessageController::createMessage(const QString& body, int roomId) 
{
	if (!_history.contains(roomId))
	{
		QtFuture::makeReadyFuture<void>().then(this, [&]() {throw "Unknown room"; });
	}
	MessageModel* model = _history[roomId];
	if (model->insertRow(0))
	{
		int tempMessageId = -(++_tempMessageCounter);
		model->setData(model->index(0), body, MessageModel::BodyRole);
		model->setData(model->index(0), tempMessageId, MessageModel::IdRole);
		model->setData(model->index(0), currentUser()->id(), MessageModel::UserIdRole);
		model->setData(model->index(0), QDateTime::currentDateTime(), MessageModel::TimeRole);
		model->setData(model->index(0), roomId, MessageModel::RoomIdRole);

		return _caller->createMessage(model->data(model->index(0), MessageModel::HashRole).toHash())
			.then([=](QVariantHash&& hash)
				{
					bool st;
					int id = MessageModel::MessageData::checkId(hash, st);
					if (st && model->idToIndex(tempMessageId).isValid())
						model->setData(model->idToIndex(tempMessageId), hash, MessageModel::HashRole);
					//else error
				});
	}
}
QFuture<void> CallerMessageController::deleteMessage(int roomId, int messId) 
{
	return _caller->deleteMessage({ {"roomId",roomId}, {"messageId",messId} }).then(this, [=](QVariantHash&& hash) {
		auto&& index = _history[roomId]->idToIndex(messId);
		if(index.isValid())
			_history[roomId]->removeRow(index.row());
		});
}
QFuture<UsersModel*> CallerUserController::getUsers(const QVariantHash& pattern, int limit)
{
	QVariantHash out(pattern);
	out["limit"] = limit;
	return _caller->getUsers(std::move(out)).then(this, [=](HashList&& list)
		{
			UsersModel* out = new UsersModel;
			for (auto& i : list)
			{
				UserInfo* info = new UserInfo;
				info->extractFromHash(i);
				out->pushUser(info);
			}
			return out;
		});
	return QtFuture::makeReadyFuture(new UsersModel);
}
QFuture <UserInfo*> CallerUserController::getUserInfo(int userId) 
{
	if (_users.contains(userId))
		return QtFuture::makeReadyFuture(_users[userId]);
	if (_calls.contains(userId))
		return _calls[userId];
	_calls[userId] = _caller->getUsers({ {"id",userId},{"limit",1}}).then(this, [=](HashList&& hash) {
		_users.insert(userId, new UserInfo(this));
		_users[userId]->extractFromHash(hash.takeFirst());
		return _users[userId];
		});
	return _calls[userId];
}
QFuture <void> CallerUserController::updateUser(const QVariantHash& data)
{
	//todo
	return QtFuture::makeReadyFuture();
}
QFuture <void> CallerUserController::deleteUser() 
{
	//todo
	return QtFuture::makeReadyFuture();
}
CallerControllerFactory::CallerControllerFactory(ServerMethodCaller* caller,QObject* parent)
	:ChatControllerFactory(parent)
	,_caller(caller)
{

}
RoomController* CallerControllerFactory::createRoomController()
{
	return new CallerRoomController(_caller);
}
MessageController* CallerControllerFactory::createMessageController()
{
	return new CallerMessageController(_caller);
}

UserController* CallerControllerFactory::createUserController() 
{
	return new CallerUserController(_caller);
}