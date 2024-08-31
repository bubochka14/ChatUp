#pragma once
#include "servermethodcaller.h"
#include "applicationfactory.h"
#include <gmock/gmock.h>
#include "chatcontroller.h"
#include "authenticationmaster.h"
class MockServerMethodCaller : public ServerMethodCaller
{
public:
	MOCK_METHOD(QFuture<QVariantHash>, getUserRooms,(int),(override));
	MOCK_METHOD(QFuture<QVariantHash>, getRoomUsers,(int),(override));
	MOCK_METHOD(QFuture<QVariantHash>, getRoomHistory,(int),(override));
	MOCK_METHOD(QFuture<QVariantHash>, getUserInfo,(int),(override));
	MOCK_METHOD(QFuture<QVariantHash>, addUserToRoom,(int , int),(override));
	MOCK_METHOD(QFuture<QVariantHash>, registerUser,(const QString& , const QString& ),(override));
	MOCK_METHOD(QFuture<QVariantHash>, loginUser,(const QString& , const QString& ),(override));
};
class MockAuthenticationMaster : public AuthenticationMaster
{
public:
	MOCK_METHOD(void, loginUser, (const QString& login, const QString& password));
	MOCK_METHOD(void, registerUser,(const QString& login, const QString& password));
};
//class MockChatController : public AbstractChatController
//{
//public:
//	MOCK_METHOD(MessageModel*, getRoomHistory, (int), (override));
//	MOCK_METHOD(UserInfo*, getUserInfo, (int), (override));
//	MOCK_METHOD(void, initializeUser, (UserInfo*), (override));
//};
class MockApplicationFactory : public ApplicationFactory
{
public:
	MOCK_METHOD(AuthenticationMaster*, createAuthenticationMaster,(),(override));
	MOCK_METHOD(ApplicationSettings*, createApplicationSettings,(),(override));
	MOCK_METHOD(AbstractWindowFactory*, createWindowFactory,(ApplicationSettings*),(override));
	MOCK_METHOD(AbstractChatController*, createChatController,(),(override));
};
class MockWindowFactory : public AbstractWindowFactory
{
public:
	MOCK_METHOD(AbstractChatWindow*, createChatWindow, (AbstractChatController*), (override));
	MOCK_METHOD(StartupWindow*, createStartupWindow,(), (override));
	MOCK_METHOD(AbstractChatWindow*, chatWindow,());
};
class MockStartup : public StartupWindow
{
public:
	MOCK_METHOD(void,show,(), (override));
	MOCK_METHOD(void,hide,(),(override));
};