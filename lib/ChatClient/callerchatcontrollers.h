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
Q_DECLARE_LOGGING_CATEGORY(LC_CROOM_CONTROLLER);
Q_DECLARE_LOGGING_CATEGORY(LC_CUSER_CONTROLLER);
Q_DECLARE_LOGGING_CATEGORY(LC_CMESSAGE_CONTROLLER);

class CHAT_CLIENT_EXPORT CallerRoomController : public RoomController
{
	Q_OBJECT;
public:
	explicit CallerRoomController(ServerMethodCaller* caller,ClientMethodDispatcher* disp, QObject* parent = nullptr);
	QFuture<void> createGroup(const QString& name) override;
	QFuture<void> createDirect(int userID) override;
	QFuture<void> addUserToRoom(int userID, int roomId) override;
	QFuture<void> deleteRoom(int id) override;
	QFuture<void> updateRoom(const QVariantHash& data) override;
	QFuture<bool> initialize(UserInfo* user) override;
protected:
	void connectToDispatcher();
private:
	ServerMethodCaller* _caller;
	ClientMethodDispatcher* _disp;
};
class CHAT_CLIENT_EXPORT CallerMessageController : public MessageController
{
	Q_OBJECT;

public:
	explicit CallerMessageController(ServerMethodCaller* caller, ClientMethodDispatcher* disp, QObject* parent = nullptr);
	QFuture <MessageModel*> getRoomHistory(int roomId) override;
	QFuture<void> updateMessage(const QVariantHash& data) override;
	QFuture<void> createMessage(const QString& body, int roomId) override;
	QFuture<void> deleteMessage(int roomid, int messId) override;
	QFuture<void> markAsRead(int roomID, size_t count) override;
	QFuture<bool> initialize(UserInfo* user) override;
protected:
	void connectToDispatcher();
private:
	ServerMethodCaller* _caller;
	ClientMethodDispatcher* _disp;
	QMap<int, MessageModel*> _history;
	int _tempMessageCounter;

};
class CHAT_CLIENT_EXPORT CallerUserController : public UserController
{
	Q_OBJECT;
;
public:
	explicit CallerUserController(ServerMethodCaller* caller, ClientMethodDispatcher* disp, QObject* parent = nullptr);
	QFuture<UsersModel*> findUsers(const QVariantHash& pattern, int limit) override;
	QFuture<UsersModel*> getUsers(const QList<int>& id) override;
	QFuture<UserInfo*> getUserInfo(int userId) override;
	QFuture<void> updateUser(const QVariantHash& data) override;
	QFuture<void> deleteUser() override;

private:
	ClientMethodDispatcher* _disp;
	ServerMethodCaller* _caller;
	QMap<int, UserInfo*> _users;
	QMap<int, QFuture<UserInfo*>> _calls;

};
class CHAT_CLIENT_EXPORT CallerControllerFactory : public ChatControllerFactory
{
public:
	explicit CallerControllerFactory(ServerMethodCaller* caller,ClientMethodDispatcher* disp, QObject* parent = nullptr);
	RoomController* createRoomController() override;
	MessageController* createMessageController() override;
	UserController* createUserController() override;
private:
	ServerMethodCaller* _caller;
	ClientMethodDispatcher* _disp;
};