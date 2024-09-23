#pragma once
#include <qobject.h>
#include "roommodel.h"
#include "messagemodel.h"
#include "clientmethoddispatcher.h"
#include "servermethodcaller.h"
#include "userinfo.h"
#include "qfuturewatcher.h"
#include "usersmodel.h"
#include <qloggingcategory.h>
#include "chatclient_include.h"
#include "chatcontroller.h"
Q_DECLARE_LOGGING_CATEGORY(LC_CHAT_CONTROLLER);
class CHAT_CLIENT_EXPORT CallerRoomController : public RoomController
{
	Q_OBJECT;
public:
	explicit CallerRoomController(ServerMethodCaller* caller, QObject* parent = nullptr);
	QFuture<void> createGroup(const QString& name) override;
	QFuture<void> createDirect(int userID) override;
	QFuture<void> addUserToRoom(int userID, int roomId) override;
	QFuture<RoomModel*> getGroups() override;
	//QFuture<QAbstractListModel*> getDirectRooms() override;

	QFuture<void> deleteRoom(int id) override;
	QFuture<void> updateRoom(const QVariantHash& data) override;
private:
	ServerMethodCaller* _caller;
	RoomModel* _groups;

};
class CHAT_CLIENT_EXPORT CallerMessageController : public MessageController
{
	Q_OBJECT;

public:
	explicit CallerMessageController(ServerMethodCaller* caller, QObject* parent = nullptr);
	QFuture <MessageModel*> getRoomHistory(int roomId) override;
	QFuture<void> updateMessage(const QVariantHash& data) override;
	QFuture<void> createMessage(const QString& body, int roomId) override;
	QFuture<void> deleteMessage(int roomid, int messId) override;
private:
	ServerMethodCaller* _caller;
	QMap<int, MessageModel*> _history;
	int _tempMessageCounter;

};
class CHAT_CLIENT_EXPORT CallerUserController : public UserController
{
	Q_OBJECT;
;
public:
	explicit CallerUserController(ServerMethodCaller* caller, QObject* parent = nullptr);
	QFuture<UsersModel*> getUsers(const QVariantHash& pattern, int limit) override;
	QFuture <UserInfo*> getUserInfo(int userId) override;
	QFuture <void> updateUser(const QVariantHash& data) override;
	QFuture <void> deleteUser() override;
	//QFuture <UsersModel*> search(const QVariantHash&, size_t conut) override;

private:
	ServerMethodCaller* _caller;
	QMap<int, UserInfo*> _users;
	QMap<int, QFuture<UserInfo*>> _calls;

};
class CHAT_CLIENT_EXPORT CallerControllerFactory : public ChatControllerFactory
{
public:
	explicit CallerControllerFactory(ServerMethodCaller* caller,QObject* parent = nullptr);
	RoomController* createRoomController() override;
	MessageController* createMessageController() override;
	UserController* createUserController() override;
private:
	ServerMethodCaller* _caller;
};