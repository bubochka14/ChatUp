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
#include <qffuture.h>
Q_DECLARE_LOGGING_CATEGORY(LC_CHAT_CONTROLLER);
//class RoomController : public QObject
//{
//	Q_OBJECT;
//	QML_ELEMENT;
//	QML_UNCREATABLE("");
//public:
//	RoomModel* userRooms() const;
//	void createRoom(const QString&) ;
//	void deleteRoom(int id) ;
//	void updateRoom(int id, const QVariantHash&);
//
//};
//class MessageController : public QObject
//{
//	Q_OBJECT;
//	QML_ELEMENT;
//	QML_UNCREATABLE("");
//public:
//	MessageModel* getRoomHistory(int roomId);
//	void updateMessage(int roomId, int messageId, const QVariantHash&);
//	void createMessage(const QString& body, int roomId);
//	void deleteMessage(int roomid, int messId);
//
//};
//class UserController : public QObject
//{
//	Q_OBJECT;
//	QML_ELEMENT;
//	QML_UNCREATABLE("");
//public:
//	UsersModel* getRoomUsers(int roomId);
//	UserInfo* getUserInfo(int userId);
//	void updateUser(int id, const QVariantHash&);
//	void deleteUser();
//
//};
class CHAT_CLIENT_EXPORT AbstractChatController : public QObject
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
	Q_PROPERTY(RoomModel* userRooms READ userRooms NOTIFY userRoomsChanged);
	Q_PROPERTY(UserInfo* currentUser READ currentUser NOTIFY currentUserChanged);
public:
	explicit AbstractChatController(QObject* parent = nullptr);
	Q_INVOKABLE virtual QFuture<MessageModel*> getRoomHistory(int roomId) = 0;
	Q_INVOKABLE virtual QFuture<UsersModel*> getRoomUsers(int id) = 0;
	Q_INVOKABLE virtual QFuture<UserInfo*> getUserInfo(int id) = 0;
	RoomModel* userRooms() const;
	UserInfo* currentUser() const;
	Q_INVOKABLE virtual QFuture<void> addUserToRoom(int roomID, int userID) = 0;
	Q_INVOKABLE virtual QFuture<void> createMessage(const QString& body, int roomId) = 0;
	Q_INVOKABLE virtual QFuture<void> createRoom(const QString& name) = 0;
	Q_INVOKABLE virtual QFuture<void> updateRoom(const QVariantHash&) = 0;
	Q_INVOKABLE virtual QFuture<void> updateMessage(const QVariantHash&) = 0;
	Q_INVOKABLE virtual QFuture<void> updateUser(const QVariantHash&) = 0;
	Q_INVOKABLE virtual QFuture<void> deleteUser() = 0;
	Q_INVOKABLE virtual QFuture<void> deleteMessage(int roomId,int messId) = 0;
	Q_INVOKABLE virtual QFuture<void> deleteRoom(int id) = 0;
	virtual ~AbstractChatController() = default;
signals:
	void errorReceived();
	void userRoomsChanged();
	void currentUserChanged();
	void initialized();
public slots:
	virtual void initializeUser(UserInfo* currentUser) =0;
protected:
	void setCurrentUser(UserInfo* other);
	void setUserRooms(RoomModel* other);
	template<class T>
	QFuture<T> makeErrorFuture(const QString& error)
	{
		return QtFuture::makeReadyFuture<void>().then(this, [&]() {throw error; });
	}
private:
	RoomModel* _userRooms;
	UserInfo* _currentUser;

};
class CHAT_CLIENT_EXPORT CallerChatController : public AbstractChatController
{
	Q_OBJECT;
public:
	explicit CallerChatController(ServerMethodCaller*caller, QObject* parent = nullptr);
	QFuture<MessageModel*> getRoomHistory(int roomId) override;
	QFuture<UserInfo*> getUserInfo(int userId) override;
	QFuture<UsersModel*> getRoomUsers(int id) override;
	QFuture<void> addUserToRoom(int roomID, int userID) override;
	QFuture<void> createMessage(const QString& body, int roomId) override;
	QFuture<void> createRoom(const QString&) override;
	QFuture<void> deleteRoom(int id) override;
	QFuture<void> updateRoom(const QVariantHash&) override;
	QFuture<void> updateMessage(const QVariantHash&) override;
	QFuture<void> deleteMessage(int roomid,int messId) override;
	QFuture<void> updateUser(const QVariantHash&) override;
	QFuture<void> deleteUser() override;

	/*void createUser(UserInfo* user);*/
public slots:
	void initializeUser(UserInfo* currentUser) override;
private:
	QMap<int, MessageModel*> _history;
	QMap<int, UserInfo*> _users;
	QMap<int, QFuture<UserInfo*>> _cals;
	ServerMethodCaller* _caller;
	int _tempMessageCounter;
};
//class WSChatController : public AbstractChatController
//{
//public:
//	explicit WSChatController(WSClient* ws, QObject* parent = nullptr);
//	MessageModel* getRoomHistory(int roomId) override;
//	UserInfo* getUserInfo(int userId) override;
//	void addUserToRoom(int roomID, int userID) override;
//	void createMessage(const QVariantHash&) override;
//	void createRoom(const QVariantHash&) override;
//	void deleteRoom(int id) override;
//	void updateRoom(const QVariantHash&) override;
//	void updateMessage(const QVariantHash&) override;
//	void deleteMessage(int id) override;
//	void updateUser(const QVariantHash&) override;
//	void deleteUser() override;
//public slots:
//	void initializeUser(UserInfo* currentUser) override;
//protected:
//	void loadRoom(int id);
//};