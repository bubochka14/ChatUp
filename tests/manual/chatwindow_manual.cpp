#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "chatclientmock.h"
#include <QFuture>
#include "app.h"
extern int globalArgc;
extern char** globalArgv;
TEST(ChatClientManualTest, ChatWindow)
{
	using namespace ::testing;
	QGuiApplication app(globalArgc, globalArgv);
	QScopedPointer<QObject> context(new QObject);
	auto defaultAppFactory = new WSApplicationFactory("", 0, context.get());
	NiceMock<MockWindowFactory> mockWindowFact;
	NiceMock<MockStartup> autoStartup;
	OfflineChatController controller;
	NiceMock<MockApplicationFactory> mockAppFactory;
	NiceMock<MockAuthenticationMaster> mockAuth;
	ON_CALL(mockAppFactory, createWindowFactory).WillByDefault(Return(&mockWindowFact));
	ON_CALL(mockAppFactory, createAuthenticationMaster).WillByDefault(Return(&mockAuth));
	ON_CALL(mockAppFactory, createChatController).WillByDefault(Return(&controller));
	ON_CALL(mockWindowFact, createStartupWindow).WillByDefault(Return(&autoStartup));
	ON_CALL(autoStartup, show).WillByDefault([&]() {
		autoStartup.registerPassed("", "");
		});
	ON_CALL(mockWindowFact, createChatWindow).WillByDefault([=](AbstractChatController* cont) 
		{return defaultAppFactory->createWindowFactory(defaultAppFactory->createApplicationSettings())->createChatWindow(cont); });
	ON_CALL(mockAuth, registerUser).WillByDefault([&]()
		{
			UserInfo* testUser = new UserInfo;
			testUser->setId(1);
			testUser->setName("test_current_user");
			testUser->setStatus(UserInfo::Online);
			emit mockAuth.authentificated(testUser);
		});
	controller.createRoom("test_room1");
	controller.createRoom("test_room2");
	controller.createMessage("message in test_room1",controller.userRooms()->data(controller.userRooms()->index(0), RoomModel::IDRole).toInt());
	controller.createMessage("message in test_room2",controller.userRooms()->data(controller.userRooms()->index(1), RoomModel::IDRole).toInt());
	ChatClient client{ &mockAppFactory };

	ASSERT_TRUE(client.run());
	app.exec();

}