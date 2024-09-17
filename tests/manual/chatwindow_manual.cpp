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
		if (!_history.contains(roomId))
			_history[roomId] = new MessageModel(this);
		return QtFuture::makeReadyFuture(_history[roomId]);
	}
	QFuture<UserInfo*> getUserInfo(int userId) override{
		return QtFuture::makeReadyFuture(_users[userId]);
	}
	QFuture<UsersModel*> getRoomUsers(int id) override{
		return QtFuture::makeReadyFuture(new UsersModel);
	}
	QFuture<void> addUserToRoom(int roomID, int userID) override{
		return QtFuture::makeReadyFuture<void>();
	}
	QFuture<void> createMessage(const QString& body, int roomId, int userId)
	{
		static int messIdCounter = 0;
		if (!_history.contains(roomId))
			_history[roomId] = new MessageModel(this);
		if (_history[roomId]->insertRow(0))
		{
			_history[roomId]->setData(_history[roomId]->index(0),
				body, MessageModel::BodyRole);
			_history[roomId]->setData(_history[roomId]->index(0),
				userId, MessageModel::UserIdRole);
			_history[roomId]->setData(_history[roomId]->index(0),
				++messIdCounter, MessageModel::IdRole);
			_history[roomId]->setData(_history[roomId]->index(0),
				QDateTime::currentDateTime(), MessageModel::TimeRole);
		}
		return QtFuture::makeReadyFuture<void>();
	}
	QFuture<void> createMessage(const QString& body, int roomId) override{
		return createMessage(body, roomId, currentUser()->id());
	}
	QFuture<void> createRoom(const QString& name) override{
		static int roomIdCounter = 0;
		if(userRooms()->insertRow(0))
		{
			userRooms()->setData(userRooms()->index(0), roomIdCounter++, RoomModel::IDRole);
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
		auto anotherUser = new UserInfo(this);
		anotherUser->setId(2);
		anotherUser->setName("another");
		addUser(anotherUser);
		createRoom("test_room1");
		createRoom("test_room2");
		int test_room1_id = userRooms()->data(userRooms()->index(0), RoomModel::IDRole).toInt();
		int test_room2_id = userRooms()->data(userRooms()->index(1), RoomModel::IDRole).toInt();

		createMessage("loading message from current user", test_room1_id, currentUser->id());
		_history[test_room1_id]->setData(_history[test_room1_id]->index(0), MessageModel::Loading, MessageModel::StatusRole);

		createMessage("sent message from current user", test_room1_id, currentUser->id());

		_history[test_room1_id]->setData(_history[test_room1_id]->index(1), MessageModel::Sent, MessageModel::StatusRole);

		createMessage("read message from current user", test_room1_id, currentUser->id());

		_history[test_room1_id]->setData(_history[test_room1_id]->index(2), MessageModel::Read, MessageModel::StatusRole);

		createMessage("error message from current user", test_room1_id, currentUser->id());

		_history[test_room1_id]->setData(_history[test_room1_id]->index(3), MessageModel::Error, MessageModel::StatusRole);

		createMessage("message in test_room2 from current user", test_room2_id, currentUser->id());
		for (size_t i = 0; i < 20; i++)
		{
			createMessage("message from another user", test_room2_id, anotherUser->id());
		}
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
	void addUser(UserInfo* user)
	{
		user->setParent(this);
		_users[user->id()] = user;
	}
private:
	QMap<int, MessageModel*> _history;
	QMap<int, UserInfo*> _users;

};
TEST(ChatClientManualTest, ChatWindow)
{
	using namespace ::testing;
	QGuiApplication qtApp(globalArgc, globalArgv);

	QScopedPointer<QObject> context(new QObject);
	QScopedPointer<AbstractWindowFactory> defaultWindowFactory(new QmlWindowFactory);
	QScopedPointer<UserInfo> testUser(new UserInfo);
	QScopedPointer<OfflineChatController> controller(new OfflineChatController);

	NiceMock<MockWindowFactory> mockWindowFact;
	NiceMock<MockStartup> autoStartup;
	NiceMock<MockNetworkFactory> mockNetFact;
	NiceMock<MockAuthenticationMaster> mockAuth;
	NiceMock<MockClientMessageDispatcher> mockDisp;

	testUser->setId(1);
	testUser->setName("test_current_user");
	testUser->setStatus(UserInfo::Online);
	emit mockAuth.authentificated(testUser.get());


	ON_CALL(mockNetFact, createDispatcher).WillByDefault(Return(&mockDisp));
	ON_CALL(mockNetFact, createAuthenticationMaster).WillByDefault(Return(&mockAuth));
	ON_CALL(mockNetFact, createChatController).WillByDefault(Return(controller.get()));
	ON_CALL(mockWindowFact, createStartupWindow).WillByDefault(Return(&autoStartup));
	ON_CALL(autoStartup, show).WillByDefault([&]() {
		autoStartup.registerPassed("", "");
		});
	ON_CALL(mockWindowFact, createChatWindow).WillByDefault([&](AbstractChatController* cont) 
		{
			return defaultWindowFactory->createChatWindow(cont);
		});
	ON_CALL(mockAuth, registerUser).WillByDefault([&](const QString&, const QString&)
		{
			emit mockAuth.authentificated(testUser.get());
		});

	App client{&mockNetFact,&mockWindowFact };
	ASSERT_TRUE(client.run());
	qtApp.exec();

}