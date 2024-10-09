#pragma once
#include "servermethodcaller.h"
#include "networkfactory.h"
#include <gmock/gmock.h>
#include "chatcontroller.h"
#include "abstractwindowfactory.h"
#include "authenticationmaster.h"
#include "startupwindow.h"
class MockServerMethodCaller : public ServerMethodCaller
{
public:
	MOCK_METHOD(QFuture<HashList>, getUserRooms,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<HashList>, getRoomUsers,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<HashList>, getRoomHistory,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, getUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, addUserToRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, createMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, createRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, registerUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, loginUser,(const QVariantHash&),(override));
};
class MockAuthenticationMaster : public AuthenticationMaster
{
public:
	MOCK_METHOD(void, loginUser, (const QString& login, const QString& password));
	MOCK_METHOD(void, registerUser,(const QString& login, const QString& password));
};
class MockRoomController : public RoomController
{
public:
	MOCK_METHOD(QFuture<void>, createGroup,(const QString& name),(override));
	MOCK_METHOD(QFuture<void>, createDirect,(int userID),(override));
	MOCK_METHOD(QFuture<void>, addUserToRoom,(int userID, int roomId),(override));
	MOCK_METHOD(QFuture<void>, deleteRoom,(int id),(override));
	MOCK_METHOD(QFuture<void>, updateRoom,(const QVariantHash& data),(override));
};
class MockMessageController : public MessageController
{
public:
	MOCK_METHOD(QFuture <MessageModel*>, getRoomHistory,(int roomId),(override));
	MOCK_METHOD(QFuture<void>, updateMessage,(const QVariantHash& data),(override));
	MOCK_METHOD(QFuture<void>, createMessage,(const QString& body, int roomId),(override));
	MOCK_METHOD(QFuture<void>, deleteMessage,(int roomid, int messId),(override));
	MOCK_METHOD(QFuture<void>, markAsRead,(int roomID, size_t count),(override));

};
class MockUserController : public UserController
{
public:
	MOCK_METHOD(QFuture<UsersModel*>, getUsers,(const QVariantHash& pattern, int limit),(override));
	MOCK_METHOD(QFuture<UserInfo*>, getUserInfo,(int userId),(override));
	MOCK_METHOD(QFuture<void>, updateUser,(const QVariantHash& data),(override));
	MOCK_METHOD(QFuture<void>, deleteUser,(),(override));
};
class MockChatControllerFactory : public ChatControllerFactory
{
public:
	MOCK_METHOD(RoomController*, createRoomController,(),(override));
	MOCK_METHOD(MessageController*, createMessageController,(),(override));
	MOCK_METHOD(UserController*, createUserController,(),(override));
}; 
class ClientMessageDispatcher : public ClientMethodDispatcher
{
public:
	MOCK_METHOD(void, addHandler,(const QString& , Handler),(override));
};
class MockNetworkFactory : public NetworkFactory
{
public:
	MOCK_METHOD(AuthenticationMaster*, createAuthenticationMaster,(),(override));
	MOCK_METHOD(ChatControllerFactory*, createControllerFactory,(),(override));
};
class MockWindowFactory : public AbstractWindowFactory
{
public:
	MOCK_METHOD(AbstractChatWindow*, createChatWindow, (RoomController*, UserController*, MessageController*), (override));
	MOCK_METHOD(StartupWindow*, createStartupWindow,(), (override));
};
class MockStartup : public StartupWindow
{
public:
	MOCK_METHOD(void,show,(), (override));
	MOCK_METHOD(void,hide,(),(override));
};