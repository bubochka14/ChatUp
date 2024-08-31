#include "chatcontroller.h"
Q_LOGGING_CATEGORY(LC_CHAT_CONTROLLER, "ChatController");
AbstractChatController::AbstractChatController(QObject* parent)
	:QObject(parent)
	,_userRooms(new RoomModel(this))
	,_currentUser(new UserInfo(this))
{

}
void AbstractChatController::setCurrentUser(UserInfo* other)
{
	if (other == _currentUser)
		return;
	UserInfo* last = _currentUser;
	_currentUser = other;
	emit currentUserChanged();
	last->deleteLater();

}
void AbstractChatController::setUserRooms(RoomModel* other)
{
	if (other == _userRooms)
		return;
	RoomModel* last = _userRooms;
	_userRooms = other;
	emit userRoomsChanged();
	last->deleteLater();
}
//void  CallerChatController::createUser(UserInfo* user) {
//	if (_users.contains(user->id()))
//	{
//		qCCritical(LC_CHAT_CONTROLLER) << "User with id " << user->id() << " already exist.";
//	}
//	_users.insert(user->id(), user);
//}
QFuture<MessageModel*> CallerChatController::getRoomHistory(int roomId) {
	if(_history.contains(roomId))
		return QtFuture::makeReadyFuture(_history.value(roomId));
	return _caller->getRoomHistory(roomId).then(this,[=](HashList&& messages) {
		_history.insert(roomId, new MessageModel(this));
		auto model = _history[roomId];
		model->insertRow(messages.count());
		for (size_t i = 0; i < messages.count(); i++)
		{
			model->setData(model->index(model->rowCount()), messages[i], MessageModel::HashRole);

		}
		return model;
		});
}
QFuture<UserInfo*> CallerChatController::getUserInfo(int userId)
{
	if (_users.contains(userId))
		return QtFuture::makeReadyFuture(_users[userId]);
	return _caller->getUserInfo(userId).then([=](QVariantHash&& hash) {
		_users.insert(userId, new UserInfo(this));
		_users[userId]->extractFromHash(hash);
		return _users[userId];
		});
}

QFuture<void> CallerChatController::addUserToRoom(int roomID, int userID)
{
	return _caller->addUserToRoom(roomID, userID).then([=](QVariantHash&& hash) {

		});
}
QFuture<void> CallerChatController::createMessage(const QString& body, int roomId)
{
	if(!_history.contains(roomId))
	{
		return makeErrorFuture<void>("Unknown room");
	}
	MessageModel* model = _history[roomId];
	if (model->insertRow(model->rowCount()))
	{
		model->setData(model->index(model->rowCount() - 1), body, MessageModel::BodyRole);
		model->setData(model->index(model->rowCount() - 1), -(++_tempMessageCounter), MessageModel::IdRole);
	}
	_caller->createMessage(model->data(model->index(model->rowCount()-1), MessageModel::HashRole).toHash())
		.then([=](QVariantHash&& hash)
			{
				model->setData(model->index(model->rowCount() - 1), hash, MessageModel::HashRole);
			});

}
QFuture<void> CallerChatController::createRoom(const QString& name)
{
	//MessageModel* model = _history[roomId];
	//if (model->insertRow(model->rowCount()))
	//{
	//	model->setData(model->index(model->rowCount() - 1), body, MessageModel::BodyRole);
	//	model->setData(model->index(model->rowCount() - 1), -(++_tempMessageCounter), MessageModel::IdRole);
	//}
	//_caller->createMessage(model->data(model->index(model->rowCount() - 1), MessageModel::HashRole).toHash())
	//	.then([=](QVariantHash&& hash)
	//		{
	//			model->setData(model->index(model->rowCount() - 1), hash, MessageModel::HashRole);
	//		});
	return QtFuture::makeReadyFuture();
}
QFuture<void> CallerChatController::deleteRoom(int id)
{
	//QModelIndex&& index = userRooms()->idToIndex(id);
	//if (!index.isValid()) 
	//	return;;
	//userRooms()->removeRow(userRooms()->idToIndex(id).row());
	return QtFuture::makeReadyFuture();

}
QFuture<void> CallerChatController::updateRoom(int id, const QVariantHash& data)
{
	//QModelIndex&& index = userRooms()->idToIndex(id);
	//if (!index.isValid())
	//	return;;
	//userRooms()->setData(index, data, RoomModel::HashRole);
	return QtFuture::makeReadyFuture();

}
QFuture<UsersModel*> CallerChatController::getRoomUsers(int id)
{
	return QtFuture::makeReadyFuture(new UsersModel);

}
QFuture<void> CallerChatController::updateMessage(int room,int  mess, const QVariantHash& data)
{
	//QModelIndex&& index = _history[room]->idToIndex(mess);
	//if (!index.isValid())
	//	return;;
	//_history[room]->setData(userRooms()->idToIndex(mess), data, RoomModel::HashRole);
	return QtFuture::makeReadyFuture();


}
QFuture<void> CallerChatController::deleteMessage(int roomId,int messId)
{/*
	QModelIndex&& index = _history[roomId]->idToIndex(messId);
	if (!index.isValid())
		return;;
	_history[roomId]->removeRow(index.row());*/
	return QtFuture::makeReadyFuture();

}
QFuture<void> CallerChatController::updateUser(int id,const QVariantHash&)
{
	return QtFuture::makeReadyFuture();

}
QFuture<void> CallerChatController::deleteUser()
{
	return QtFuture::makeReadyFuture();

}
//void OfflineChatController::loadRoom(int id)
//{
//	//_history.insert(id, new MessageModel(this));
//	//_caller->getRoomHistory(id).then([=](QFuture<QVariantHash> messages)
//	//	{
//	//		size_t resultCount = messages.resultCount();
//	//		for (size_t i = 0; i < resultCount; i++)
//	//		{
//	//			_history.value(id)->setData(_history.value(id)->index(i), messages.takeResult(), MessageModel::HashRole);
//	//		}
//	//	});
//}
void CallerChatController::initializeUser(UserInfo* currentUser)
{
	setCurrentUser(currentUser);
	_caller->getUserRooms(currentUser->id())
		.then([=](HashList&& rooms)
			{
				userRooms()->insertRows(0, rooms.count());
				for (size_t i = 0; i < rooms.count(); i++)
				{
					userRooms()->setData(userRooms()->index(i, 0), rooms.takeFirst(), RoomModel::HashRole);
				}
				emit initialized();
			});
}
RoomModel* AbstractChatController::userRooms() const
{
	return _userRooms;
}
UserInfo* AbstractChatController::currentUser() const
{
	return _currentUser;
}
CallerChatController::CallerChatController(ServerMethodCaller* caller, QObject* parent)
	:AbstractChatController(parent)
	,_caller(caller)
	,_tempMessageCounter(0)
{}
