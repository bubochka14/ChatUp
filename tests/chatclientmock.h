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
	MOCK_METHOD(QFuture<HashList>, getUserRooms,(int id),(override));
	MOCK_METHOD(QFuture<HashList>, getRoomUsers,(int id),(override));
	MOCK_METHOD(QFuture<HashList>, getRoomHistory,(int id),(override));
	MOCK_METHOD(QFuture<QVariantHash>, getUserInfo,(int id),(override));
	MOCK_METHOD(QFuture<QVariantHash>, addUserToRoom,(int roomID, int userID),(override));
	MOCK_METHOD(QFuture<QVariantHash>, createMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, createRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteRoom,(int id),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteMessage,(int roomId, int messageId),(override));
	MOCK_METHOD(QFuture<QVariantHash>, updateUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<QVariantHash>, deleteUser,(int id),(override));
	MOCK_METHOD(QFuture<QVariantHash>, registerUser,(const QString& , const QString& ),(override));
	MOCK_METHOD(QFuture<QVariantHash>, loginUser,(const QString& , const QString& ),(override));
};
class MockAuthenticationMaster : public AuthenticationMaster
{
public:
	MOCK_METHOD(void, loginUser, (const QString& login, const QString& password));
	MOCK_METHOD(void, registerUser,(const QString& login, const QString& password));
};
class MockChatController : public AbstractChatController
{
public:
	MOCK_METHOD(QFuture<MessageModel*>, getRoomHistory,(int),(override));
	MOCK_METHOD(QFuture<UsersModel*>, getRoomUsers,(int),(override));
	MOCK_METHOD(QFuture<UserInfo*>, getUserInfo,(int),(override));
	MOCK_METHOD(QFuture<void>, addUserToRoom,(int, int),(override));
	MOCK_METHOD(QFuture<void>, createMessage,(const QString&, int),(override));
	MOCK_METHOD(QFuture<void>, createRoom,(const QString&),(override));
	MOCK_METHOD(QFuture<void>, updateRoom,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<void>, updateMessage,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<void>, updateUser,(const QVariantHash&),(override));
	MOCK_METHOD(QFuture<void>, deleteUser,(),(override));
	MOCK_METHOD(QFuture<void>, deleteMessage,(int,int),(override));
	MOCK_METHOD(QFuture<void>, deleteRoom,(int),(override));
	void setUserRooms(RoomModel*);
	void setCurrentUser(UserInfo*);
};
class MockClientMessageDispatcher : public ClientMethodDispatcher
{
public:
	MOCK_METHOD(void, addCustomHandler,(const QString& , Handler),(override));
};
class MockNetworkFactory : public NetworkFactory
{
public:
	MOCK_METHOD(AuthenticationMaster*, createAuthenticationMaster,(),(override));
	MOCK_METHOD(AbstractChatController*, createChatController,(),(override));
	MOCK_METHOD(ClientMethodDispatcher*, createDispatcher,(),(override));
};
class MockWindowFactory : public AbstractWindowFactory
{
public:
	MOCK_METHOD(AbstractChatWindow*, createChatWindow, (AbstractChatController*), (override));
	MOCK_METHOD(StartupWindow*, createStartupWindow,(), (override));
};
class MockStartup : public StartupWindow
{
public:
	MOCK_METHOD(void,show,(), (override));
	MOCK_METHOD(void,hide,(),(override));
};