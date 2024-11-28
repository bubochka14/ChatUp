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
#include "network_include.h"
Q_DECLARE_LOGGING_CATEGORY(LC_CHAT_CONTROLLER);
class CC_NETWORK_EXPORT AbstractChatController : public QObject
{
	Q_OBJECT;
	Q_PROPERTY(UserInfo* currentUser READ currentUser WRITE setCurrentUser	NOTIFY currentUserChanged);
	Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged);
public:
	explicit AbstractChatController(QObject* parent = nullptr);
	UserInfo* currentUser() const;
	virtual QFuture<bool> initialize(UserInfo*);
	void logout();
	bool initialized() const;
protected:
	void setCurrentUser(UserInfo* other);
	void setInitialized(bool ohter);
	virtual void reset();
signals:
	void currentUserChanged();
	void initializedChanged();
private:
	UserInfo* _currentUser;
	bool _initialized;

};
class CC_NETWORK_EXPORT RoomController : public AbstractChatController
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("Abstract class");
	Q_PROPERTY(RoomModel* userGroups READ userGroups NOTIFY userGroupsChanged);
	Q_PROPERTY(RoomModel* userDirects READ userDirects NOTIFY userDirectsChanged);
public:
	explicit RoomController(QObject* parent = nullptr);
	Q_INVOKABLE virtual QFuture<void> createGroup(const QString& name) = 0;
	Q_INVOKABLE virtual QFuture<void> createDirect(int userID) = 0;
	Q_INVOKABLE virtual QFuture<void> addUserToRoom(int userID, int roomId) =0;
	Q_INVOKABLE virtual QFuture<void> deleteRoom(int id) = 0;
	Q_INVOKABLE virtual QFuture<void> updateRoom(const QVariantHash& data) = 0;
	RoomModel* userGroups() const;
	RoomModel* userDirects() const;
signals:
	void userGroupsChanged();
	void userDirectsChanged();
protected:
	virtual void reset() override;
	void setUserGroups(RoomModel* other);
	void setDirectRooms(RoomModel* other);
	RoomModel* findModel(int roomID) const;
private:
	RoomModel* _groups;
	RoomModel* _directs;

};
class CC_NETWORK_EXPORT MessageController : public AbstractChatController
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("Abstract class");
public:
	explicit MessageController(QObject* parent = nullptr);
	Q_INVOKABLE virtual QFuture <MessageModel*> getRoomHistory(int roomId) = 0;
	Q_INVOKABLE virtual QFuture<void> updateMessage(const QVariantHash& data) = 0;
	Q_INVOKABLE virtual QFuture<void> createMessage(const QString& body, int roomId) = 0;
	Q_INVOKABLE virtual QFuture<void> deleteMessage(int roomid, int messId) = 0;
	Q_INVOKABLE virtual QFuture<void> markAsRead(int roomID, size_t count) = 0;

};
class CC_NETWORK_EXPORT UserController : public AbstractChatController
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("Abstract class");
public:
	explicit UserController(QObject* parent = nullptr);
	Q_INVOKABLE virtual QFuture<UsersModel*> findUsers(const QVariantHash& pattern, int limit) = 0;
	Q_INVOKABLE virtual QFuture<UserInfo*> getUserInfo(int userId) = 0;
	Q_INVOKABLE virtual QFuture<void> updateUser(const QVariantHash& data) = 0;
	Q_INVOKABLE virtual QFuture<void> deleteUser() = 0;
	//Q_INVOKABLE virtual QFuture<UsersModel*> search(const QVariantHash&,size_t conut) = 0;
};
class CC_NETWORK_EXPORT ChatControllerFactory : public QObject
{
	Q_OBJECT;
public:
	virtual RoomController* createRoomController() =0;
	virtual MessageController* createMessageController() =0 ;
	virtual UserController* createUserController() =0;
	virtual ~ChatControllerFactory() = default;
protected:
	explicit ChatControllerFactory(QObject* parent = nullptr);
};