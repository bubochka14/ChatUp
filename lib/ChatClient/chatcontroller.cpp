#include "chatcontroller.h"
Q_LOGGING_CATEGORY(LC_CHAT_CONTROLLER, "ChatController");
AbstractChatController::AbstractChatController(QObject* parent)
	:QObject(parent)
	,_currentUser(new UserInfo(this))
{}
void AbstractChatController::setCurrentUser(UserInfo* other)
{
	if (_currentUser == other)
		return;
	_currentUser = other;
	emit currentUserChanged();
}
UserInfo* AbstractChatController::currentUser() const
{
	return _currentUser;
}
RoomController::RoomController(QObject* parent):AbstractChatController(parent){}
UserController::UserController(QObject* parent):AbstractChatController(parent) {}
MessageController::MessageController(QObject * parent):AbstractChatController(parent) {}
ChatControllerFactory::ChatControllerFactory(QObject* parent): QObject(parent){}

//
//AbstractChatController::AbstractChatController(QObject* parent)
//	:QObject(parent)
//	,_userRooms(new RoomModel(this))
//	,_currentUser(new UserInfo(this))
//{
//}
//void AbstractChatController::setCurrentUser(UserInfo* other)
//{
//	if (other == _currentUser)
//		return;
//	UserInfo* last = _currentUser;
//	_currentUser = other;
//	emit currentUserChanged();
//	last->deleteLater();
//
//}
//void AbstractChatController::setUserRooms(RoomModel* other)
//{
//	if (other == _userRooms)
//		return;
//	RoomModel* last = _userRooms;
//	_userRooms = other;
//	emit userRoomsChanged();
//	last->deleteLater();
//}
////void  CallerChatController::createUser(UserInfo* user) {
////	if (_users.contains(user->id()))
////	{
////		qCCritical(LC_CHAT_CONTROLLER) << "User with id " << user->id() << " already exist.";
////	}
////	_users.insert(user->id(), user);
////}
//QFuture<MessageModel*> CallerChatController::getRoomHistory(int roomId) {
//	if(_history.contains(roomId))
//		return QtFuture::makeReadyFuture(_history.value(roomId));
//	return _caller->getRoomHistory(roomId).then(this,[=](HashList&& messages) {
//		_history.insert(roomId, new MessageModel(this));
//		auto model = _history[roomId];
//		model->insertRows(0,messages.count());
//		for (size_t i = 0; i < messages.count(); i++)
//		{
//			model->setData(model->index(i), messages[i], MessageModel::HashRole);
//
//		}
//		return model;
//		});
//}
//QFuture<UserInfo*> CallerChatController::getUserInfo(int userId)
//{
//	if (_users.contains(userId))
//		return QtFuture::makeReadyFuture(_users[userId]);
//	if (_cals.contains(userId))
//		return _cals[userId];
//	_cals[userId] = _caller->getUserInfo(userId).then(this,[=](QVariantHash&& hash) {
//		_users.insert(userId, new UserInfo(this));
//		_users[userId]->extractFromHash(hash);
//		return _users[userId];
//		});
//	return _cals[userId];
//}
//
//QFuture<void> CallerChatController::addUserToRoom(int roomID, int userID)
//{
//	return _caller->addUserToRoom(roomID, userID).then([=](QVariantHash&& hash) {
//
//		});
//}
//QFuture<void> CallerChatController::createMessage(const QString& body, int roomId)
//{
//	if(!_history.contains(roomId))
//	{
//		return makeErrorFuture<void>("Unknown room");
//	}
//	MessageModel* model = _history[roomId];
//	if (model->insertRow(0))
//	{
//		int tempMessageId = -(++_tempMessageCounter);
//		model->setData(model->index(0), body, MessageModel::BodyRole);
//		model->setData(model->index(0), tempMessageId, MessageModel::IdRole);
//		model->setData(model->index(0), currentUser()->id(), MessageModel::UserIdRole);
//		model->setData(model->index(0), QDateTime::currentDateTime(), MessageModel::TimeRole);
//		model->setData(model->index(0), roomId, MessageModel::RoomIdRole);
//
//		return _caller->createMessage(model->data(model->index(0), MessageModel::HashRole).toHash())
//			.then([=](QVariantHash&& hash)
//				{
//					bool st;
//					int id = MessageModel::MessageData::checkId(hash, st);
//					if (st && model->idToIndex(tempMessageId).isValid())
//						model->setData(model->idToIndex(tempMessageId), hash, MessageModel::HashRole);
//					//else error
//				});
//	}
//
//}
//QFuture<void> CallerChatController::createRoom(const QString& name)
//{
//	return _caller->createRoom({ {"name",name} }).then(this,[=](QVariantHash&& hash) {
//		userRooms()->insertRow(0);
//		userRooms()->setData(userRooms()->index(0), hash, RoomModel::HashRole);
//		});
//}
//QFuture<void> CallerChatController::deleteRoom(int id)
//{
//	return _caller->deleteRoom(id).then([=](QVariantHash&& hash) {
//		userRooms()->removeRow(userRooms()->idToIndex(id).row());
//		});
//
//}
//QFuture<void> CallerChatController::updateRoom(const QVariantHash& data)
//{
//	return _caller->updateRoom(data).then(this, [=](QVariantHash&& hash) {
//		userRooms()->insertRow(0);
//		userRooms()->setData(userRooms()->index(0), hash, RoomModel::HashRole);
//		});
//
//}
//QFuture<UsersModel*> CallerChatController::getRoomUsers(int id)
//{
//	//todo
//	return QtFuture::makeReadyFuture(new UsersModel);
//
//}
//QFuture<void> CallerChatController::updateMessage(const QVariantHash& data)
//{
//	return _caller->updateMessage(data).then(this, [=](QVariantHash&& hash) {
//		auto model = _history[data["roomId"].toUInt()];
//		model->setData(model->idToIndex(data["id"].toInt()),hash,MessageModel::HashRole);
//		});
//
//
//}
//QFuture<void> CallerChatController::deleteMessage(int roomId,int messId)
//{
//	return _caller->deleteMessage(roomId,messId).then(this, [=](QVariantHash&& hash) {
//		_history[roomId]->removeRow(_history[roomId]->idToIndex(messId).row());
//		});
//}
//QFuture<void> CallerChatController::updateUser(const QVariantHash&)
//{
//	//todo
//	return QtFuture::makeReadyFuture();
//
//}
//QFuture<void> CallerChatController::deleteUser()
//{
//	//todo
//	return QtFuture::makeReadyFuture();
//
//}
//void CallerChatController::initializeUser(UserInfo* currentUser)
//{
//	setCurrentUser(currentUser);
//	_caller->getUserRooms(currentUser->id())
//		.then([=](HashList&& rooms)
//			{
//				userRooms()->insertRows(0, rooms.count());
//				for (size_t i = 0; i < rooms.count(); i++)
//				{
//					userRooms()->setData(userRooms()->index(i, 0), rooms.takeFirst(), RoomModel::HashRole);
//				}
//				emit initialized();
//			});
//}
//RoomModel* AbstractChatController::userRooms() const
//{
//	return _userRooms;
//}
//UserInfo* AbstractChatController::currentUser() const
//{
//	return _currentUser;
//}
//CallerChatController::CallerChatController(ServerMethodCaller* caller, QObject* parent)
//	:AbstractChatController(parent)
//	,_caller(caller)
//	,_tempMessageCounter(0)
//{}
