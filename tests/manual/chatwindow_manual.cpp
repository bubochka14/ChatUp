#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "chatclientmock.h"
#include <QFuture>
#include "app.h"
extern int globalArgc;
extern char** globalArgv;
class OfflineChatController : public AbstractChatController
{
public:
	QFuture<MessageModel*> getRoomHistory(int roomId) override {
		return QtFuture::makeReadyFuture(_history[roomId]);
	}
	QFuture<UserInfo*> getUserInfo(int userId) override{
		qDebug() << _users[userId] << userId;
		return QtFuture::makeReadyFuture(_users[userId]);
	}
	QFuture<UsersModel*> getRoomUsers(int id) override{
		return QtFuture::makeReadyFuture(new UsersModel);
	}
	QFuture<void> addUserToRoom(int roomID, int userID) override{
		return QtFuture::makeReadyFuture<void>();
	}
	QFuture<void> createMessage(const QString& body, int roomId) override{
		static int messIdCounter = 0;
		if (!_history.contains(roomId))
			_history[roomId] = new MessageModel(this);
		if(_history[roomId]->insertRow(_history[roomId]->rowCount()))
		{
			_history[roomId]->setData(_history[roomId]->index(_history[roomId]->rowCount() - 1),
				body, MessageModel::BodyRole);
			_history[roomId]->setData(_history[roomId]->index(_history[roomId]->rowCount() - 1),
				1, MessageModel::UserIdRole);
			_history[roomId]->setData(_history[roomId]->index(_history[roomId]->rowCount() - 1),
				++messIdCounter, MessageModel::IdRole);
			_history[roomId]->setData(_history[roomId]->index(_history[roomId]->rowCount() - 1),
				QDateTime::currentDateTime(), MessageModel::TimeRole);
		}
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> createRoom(const QString& name) override{
		static int roomIdCounter = 0;
		if(userRooms()->insertRow(0))
		{
			userRooms()->setData(userRooms()->index(0), ++roomIdCounter, RoomModel::IDRole);
			userRooms()->setData(userRooms()->index(0), name, RoomModel::NameRole);
		}
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> deleteRoom(int id) override{
		userRooms()->removeRow(userRooms()->idToIndex(id).row());
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> updateRoom(const QVariantHash& data) override {
		bool st;
		int id = RoomModel::RoomData::checkId(data, st);
		if (st)
			userRooms()->setData(userRooms()->idToIndex(id), data, RoomModel::HashRole);
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> updateMessage( const QVariantHash&data) override{
		bool st;
		int id = MessageModel::MessageData::checkId(data, st);
		if (!st)
			return QtFuture::makeReadyFuture<void>();
		int roomId = MessageModel::MessageData::checkId(data, st);
		if (st)
			_history[roomId]->setData(_history[roomId]->idToIndex(id), data, RoomModel::HashRole);
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> deleteMessage(int roomId, int messId) override{
		_history[roomId]->removeRow(_history[roomId]->idToIndex(roomId).row());
		return QtFuture::makeReadyFuture<void>();

	}
	QFuture<void> updateUser(const QVariantHash& data) override{
		bool st;
		int id = UserInfo::checkId(data, st);
		if (st)
			_users[id]->extractFromHash(data);
		return QtFuture::makeReadyFuture<void>();
	}
	QFuture<void> deleteUser() override{
		return QtFuture::makeReadyFuture<void>();

	}
	void initializeUser(UserInfo* currentUser) override{
		setCurrentUser(currentUser);
		emit initialized();
	}
	void setCurrentUser(UserInfo* user)
	{
		_users.insert(user->id(), user);
		AbstractChatController::setCurrentUser(user);
	}
	void setRoomModel(RoomModel* model)
	{
		AbstractChatController::setUserRooms(model);
	}
private:
	QMap<int, MessageModel*> _history;
	QMap<int, UserInfo*> _users;

};
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
	NiceMock<MockClientMessageDispatcher> mockDisp;
	ON_CALL(mockAppFactory, createWindowFactory).WillByDefault(Return(&mockWindowFact));
	ON_CALL(mockAppFactory, createDispatcher).WillByDefault(Return(&mockDisp));
	ON_CALL(mockAppFactory, createAuthenticationMaster).WillByDefault(Return(&mockAuth));
	ON_CALL(mockAppFactory, createChatController).WillByDefault(Return(&controller));
	ON_CALL(mockWindowFact, createStartupWindow).WillByDefault(Return(&autoStartup));
	ON_CALL(autoStartup, show).WillByDefault([&]() {
		autoStartup.registerPassed("", "");
		});
	ON_CALL(mockWindowFact, createChatWindow).WillByDefault([=](AbstractChatController* cont) 
		{return defaultAppFactory->createWindowFactory(
			defaultAppFactory->createApplicationSettings())->createChatWindow(cont); });
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
	controller.createMessage("message in test_room1",controller.userRooms()->data(
		controller.userRooms()->index(0), RoomModel::IDRole).toInt());
	controller.createMessage("message in test_room2",controller.userRooms()->data(
		controller.userRooms()->index(1), RoomModel::IDRole).toInt());
	ChatClient client{ &mockAppFactory };

	ASSERT_TRUE(client.run());
	app.exec();

}