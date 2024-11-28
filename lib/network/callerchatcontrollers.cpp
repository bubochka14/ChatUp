#include "callerchatcontrollers.h"
Q_LOGGING_CATEGORY(LC_CROOM_CONTROLLER, "CallerRoomController");
Q_LOGGING_CATEGORY(LC_CUSER_CONTROLLER, "CallerUserController");
Q_LOGGING_CATEGORY(LC_CMESSAGE_CONTROLLER, "CallerMessageController");
CallerRoomController::CallerRoomController(ServerMethodCaller* caller, ClientMethodDispatcher* disp, QObject* parent)
	:RoomController(parent)
	, _caller(caller)
	, _disp(disp)
{

}
CallerMessageController::CallerMessageController(ServerMethodCaller* caller, ClientMethodDispatcher* disp, QObject* parent)
	:MessageController(parent)
	, _caller(caller)
	, _tempMessageCounter(0)
	, _disp(disp)

{

}
CallerUserController::CallerUserController(ServerMethodCaller* caller, ClientMethodDispatcher* disp, QObject* parent)
	:UserController(parent)
	,_caller(caller)
	,_disp(disp)
{

}
void CallerRoomController::connectToDispatcher() {
	_disp->addHandler("addRoom", [=](const QVariantHash& hash) {
		userGroups()->insertRow(0);
		userGroups()->setData(userGroups()->index(0), hash, RoomModel::HashRole);
		});
	_disp->addHandler("updateRoom", [=](const QVariantHash& hash) {
		if (!hash.contains("roomID"))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "received updateRoom data doesn`t have roomID field";
			return;
		}
		RoomModel* model = findModel(hash["roomID"].toInt());
		if (!model)
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
			return;
		}
		model->setData(model->idToIndex(hash["roomID"].toInt()), hash, RoomModel::HashRole);
		});
}
QFuture<void> CallerRoomController::createGroup(const QString& name) 
{
	return _caller->createRoom({ {"name",name},{"type","group"} }).then(this, [=](QVariantHash&& hash) {
		userGroups()->insertRow(0);
		userGroups()->setData(userGroups()->index(0), hash, RoomModel::HashRole);
		});
}
QFuture<void> CallerRoomController::createDirect(int userID) 
{
	//todo
	return QtFuture::makeReadyFuture();
}
QFuture<bool> CallerRoomController::initialize(UserInfo* user)
{
	if (initialized() && currentUser() == user)
		return QtFuture::makeReadyValueFuture(true);
	return _caller->getUserRooms({ {"userID",user->id()},{"type","group"} })
		.then([=](HashList&& rooms)
			{
				userGroups()->insertRows(0, rooms.count());
				size_t roomsCount = rooms.count();
				for (size_t i = 0; i < roomsCount; i++)
				{
					userGroups()->setData(userGroups()->index(0 + i), rooms.takeFirst(), RoomModel::HashRole);
				}
				RoomController::initialize(user);
				connectToDispatcher();
				return true;
			});
}
QFuture<void> CallerRoomController::addUserToRoom(int userID, int roomID)
{
	return _caller->addUserToRoom({ {"roomID",roomID}, {"userID",userID} }).then([](QVariantHash&&){});
}
QFuture<bool> CallerMessageController::initialize(UserInfo* user)
{
	connectToDispatcher();
	return 	MessageController::initialize(user);
}
void CallerMessageController::connectToDispatcher()
{
	_disp->addHandler("postMessage", [=](const QVariantHash& hash) {
		if (!hash.contains("roomID"))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "received postMessage data doesn`t have roomID field";
			return;
		}
		int roomID = hash["roomID"].toInt();
		if (!_history.contains(roomID))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
			return;
		}
		MessageModel* model = _history[roomID];
		if (model->insertRow(0))
			model->setData(model->index(0), hash, MessageModel::HashRole);
	});
	_disp->addHandler("updateMessage", [=](const QVariantHash& hash) {
		if (!hash.contains("roomID"))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "received updateMessage data doesn`t have roomID field";
			return;
		}
		int roomID = hash["roomID"].toInt();
		if (!_history.contains(roomID))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
			return;
		}
		MessageModel* model = _history[roomID];
			model->setData(model->index(0), hash, MessageModel::HashRole);
		});
	_disp->addHandler("updateReadCount", [=](const QVariantHash& hash) {
		if (!hash.contains("roomID"))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "received updateReadCount data doesn`t have roomID field";
			return;
		}
		int roomID = hash["roomID"].toInt();
		if (!_history.contains(roomID))
		{
			qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
			return;
		}
		MessageModel* model = _history[roomID];
		if(hash.contains("maxCount"))
			model->setForeignReadMessagesCount(hash["maxCount"].toUInt());
		if (hash.contains("userCount"))
			model->setUserReadMessagesCount(hash["userCount"].toUInt());
		});

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

QFuture <MessageModel*> CallerMessageController::getRoomHistory(int roomID)
{
	if (_history.contains(roomID))
		return QtFuture::makeReadyFuture(_history.value(roomID));
	MessageModel* newHistory = new MessageModel(currentUser()->id(), this);
	QFuture<HashList> roomMessagesFuture = _caller->getRoomHistory({ { "roomID", roomID } });
	QFuture<QVariantHash> userReadCountFuture = _caller->getReadMessagesCount({ { "roomID", roomID } });
	return QtFuture::whenAll(roomMessagesFuture, userReadCountFuture)
		.then([=](QList<std::variant<QFuture<HashList>, QFuture<QVariantHash>>>&& results) {
		if (results.length() < 1)
			return newHistory;
		auto&& result = results.takeFirst();
		HashList&& messages = std::get<QFuture<HashList>>(result).takeResult();
		newHistory->insertRows(newHistory->rowCount(), messages.count());
		size_t messageIndex = 0;
		for (auto begin = std::make_move_iterator(messages.rbegin()),
			end = std::make_move_iterator(messages.rend());
			begin != end; ++begin)
		{
			newHistory->setData(newHistory->index(messageIndex), *begin, MessageModel::HashRole);
			newHistory->setData(newHistory->index(messageIndex++), MessageModel::Sent, MessageModel::StatusRole);
		}
		if (_history.contains(roomID))
			_history[roomID]->deleteLater();
		_history.insert(roomID, newHistory);
		result = results.takeFirst();
		QFuture<QVariantHash>&& readCountInfoFuture = std::get<QFuture<QVariantHash>>(std::move(result));
		QVariantHash&& readCountInfo = readCountInfoFuture.takeResult();

		newHistory->setUserReadMessagesCount(readCountInfo.value("userCount").toUInt());
		newHistory->setForeignReadMessagesCount(readCountInfo.value("maxCount").toUInt());
		return newHistory;
	});
}
QFuture<void> CallerMessageController::updateMessage(const QVariantHash& data)
{
	QVariantHash out(data);
	return _caller->updateMessage(std::move(out)).then(this, [=](QVariantHash&& res) {
		auto model = _history[res["roomID"].toUInt()];
		auto&& index = model->idToIndex(res["id"].toInt());
		if(index.isValid())
			model->setData(index, res, MessageModel::HashRole);
		});
}
QFuture<void> CallerMessageController::markAsRead(int roomID, size_t count)
{
	if (!_history.contains(roomID))
	{
		qCWarning(LC_CMESSAGE_CONTROLLER) << "Unknown roomID received";
		return QtFuture::makeReadyFuture();
	}
	if (_history[roomID]->userReadMessagesCount() >= count)
		return QtFuture::makeReadyFuture();
	_history[roomID]->setUserReadMessagesCount(count);
	return _caller->setReadMessagesCount({ {"roomID",roomID},{"userID",currentUser()->id()}, { "count",count }})
		.then([](QVariantHash&& res) {
				
			});
}

QFuture<void> CallerMessageController::createMessage(const QString& body, int roomID) 
{
	if (!_history.contains(roomID))
	{
		_history[roomID] = new MessageModel(currentUser()->id(), this);
	}
	MessageModel* model = _history[roomID];
	if (model->insertRow(0))
	{
		int tempMessageId = -(++_tempMessageCounter);
		model->setData(model->index(0), body, MessageModel::BodyRole);
		model->setData(model->index(0), tempMessageId, MessageModel::IdRole);
		model->setData(model->index(0), currentUser()->id(), MessageModel::UserIdRole);
		model->setData(model->index(0), QDateTime::currentDateTime(), MessageModel::TimeRole);
		model->addSpecialMessageStatus(0, MessageModel::Loading);
		QVariantHash&& messageData = model->data(model->index(0), MessageModel::HashRole).toHash();
		messageData.insert({ {"roomID",roomID} });
		return _caller->createMessage(messageData)
			.then([=](QVariantHash&& hash)
				{
					if (!model->idToIndex(tempMessageId).isValid())
						return;
					model->removeSpecialMessageStatus(model->idToIndex(tempMessageId).row());
					model->setData(model->idToIndex(tempMessageId), hash, MessageModel::HashRole);
					
				});
	}
}
QFuture<void> CallerMessageController::deleteMessage(int roomID, int messId) 
{
	return _caller->deleteMessage({ {"roomID",roomID}, {"messageID",messId} }).then(this, [=](QVariantHash&& hash) {
		auto&& index = _history[roomID]->idToIndex(messId);
		if(index.isValid())
			_history[roomID]->removeRow(index.row());
	});
}
void CallerMessageController::reset()
{
	_tempMessageCounter = 0;
	for (auto& i : _history)
	{
		i->deleteLater();
	}
	_history.clear();
}

QFuture<UsersModel*> CallerUserController::findUsers(const QVariantHash& pattern, int limit)
{
	QVariantHash out(pattern);
	out["limit"] = limit;
	out["regexp"] = true;
	return _caller->findUsers(out).then(this, [=](HashList&& list)
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
	_calls[userId] = _caller->getUsers({ { "id",userId} }).then(this, [=](QVariantHash&& hash) {
		_users.insert(userId, new UserInfo(this));
		_users[userId]->extractFromHash(hash);
		return _users[userId];
		});
	return _calls[userId];
}
QFuture <void> CallerUserController::updateUser(const QVariantHash& data)
{
	//todo
	return QtFuture::makeReadyFuture();
}
void CallerUserController::reset()
{
	for (auto& i : _users)
	{
		i->deleteLater();
	}
	_users.clear();
	for (auto& i : _calls)
	{
		i.suspend();
	}
	_calls.clear();

}

QFuture <void> CallerUserController::deleteUser() 
{
	//todo
	return QtFuture::makeReadyFuture();
}
CallerControllerFactory::CallerControllerFactory(ServerMethodCaller* caller, ClientMethodDispatcher* dispatcher, QObject* parent)
	:ChatControllerFactory(parent)
	,_caller(caller)
	,_disp(dispatcher)
{

}
RoomController* CallerControllerFactory::createRoomController()
{
	return new CallerRoomController(_caller,_disp);
}
MessageController* CallerControllerFactory::createMessageController()
{
	return new CallerMessageController(_caller, _disp);
}

UserController* CallerControllerFactory::createUserController() 
{
	return new CallerUserController(_caller, _disp);
}