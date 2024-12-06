#include "chatcontroller.h"
Q_LOGGING_CATEGORY(LC_CHAT_CONTROLLER, "ChatController");
AbstractChatController::AbstractChatController(QObject* parent)
	:QObject(parent)
	,_currentUser(new UserInfo(this))
	,_initialized(false)
{}
void AbstractChatController::logout()
{
	reset();
	setCurrentUser(nullptr);
	setInitialized(false);
}
void  AbstractChatController::reset(){}

bool AbstractChatController::initialized() const
{
	return _initialized;
}
QFuture<bool> AbstractChatController::initialize(UserInfo* user) {
	setCurrentUser(user);
	setInitialized(true);
	return QtFuture::makeReadyFuture(true);
}

void  AbstractChatController::setInitialized(bool other)
{
	if (_initialized == other)
		return;
	_initialized = other;
	emit initializedChanged();
}

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
void RoomController::setUserGroups(RoomModel* other)
{
	if (_groups == other)
		return;
	_groups = other;
	emit userGroupsChanged();
}
void RoomController::reset()
{
	_groups->removeRows(0,_groups->rowCount());
	_directs->removeRows(0, _directs->rowCount());
}

void RoomController::setDirectRooms(RoomModel* other)
{
	if (_directs == other)
		return;
	_directs = other;
	emit userDirectsChanged();
}
RoomController::RoomController(QObject* parent)
	:AbstractChatController(parent)
	,_groups(new RoomModel(this))
	,_directs(new RoomModel(this))
{}
RoomModel* RoomController::findModel(int roomID) const
{
	if (_groups->idToIndex(roomID).isValid())
		return _groups;
	if (_directs->idToIndex(roomID).isValid())
		return _directs;
	return nullptr;
}
RoomModel* RoomController::userGroups() const
{
	return _groups;
}
RoomModel* RoomController::userDirects() const
{
	return _directs;
}
UserController::UserController(QObject* parent):AbstractChatController(parent) {}
MessageController::MessageController(QObject* parent) :AbstractChatController(parent) {}
ControllerManager::ControllerManager(QObject* parent) :QObject(parent) {}
