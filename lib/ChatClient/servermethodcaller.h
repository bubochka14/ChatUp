#pragma once
#include <qobject.h>
#include "wsclient.h"
#include "chatroomstructs.h"
#include "messageconstructor.h"
#include <QNetworkReply>
#include <QMetaEnum>
#include <functional>
#include <QFuture>
template <class T>
struct MethodReturn
{
	NetworkError error;
	QSharedPointer<T> data;

};
class MethodCallFailure : public std::exception
{
	QString _message;
	std::string _stdMessage;
public:
	explicit MethodCallFailure(const QString& message);
	const char* what() const noexcept override;
	QString errorString() const noexcept;
};
struct UserCredentials
{
	Q_GADGET;
	Q_PROPERTY(QString username MEMBER username);
	Q_PROPERTY(QString password MEMBER password);
public:
	QString username;
	QString password;
	UserCredentials(const QString username, const QString& password);
};
class ServerMethodCaller : public QObject
{
	Q_OBJECT;
	WSClient* _transport;
public:
	explicit ServerMethodCaller(WSClient* transport, QObject* parent = nullptr);
	QFuture<RoomList> getUserRooms(int userID);
	//QFuture<MethodReturn<UserInfo>> getRoomUsers(const QString& userToken, int roomID);
	QFuture<QList<ChatRoomMessage>> getRoomHistory(const QString& userToken, int roomID);
	QFuture<UserInfo> getUserInfo(const QString& username);
	QFuture<UserInfo> getUserInfo(int id);
	QFuture<UserInfo> getCurrentUserInfo(const QString& userToken);
	//QFuture<QStringList> getAvailableServerMethods();
	QFuture<bool> sendChatMessage(const QString& userToken, int roomID, const QString& message);
	//QFuture<bool> addUserToRoom(const QString& UserToken, int roomID, int usserID);
	QFuture<QString> registerUser(const UserCredentials& cr);
	QFuture<QString> loginUser(const UserCredentials& cr);


	template<class T>
	QFuture<T> customMethod(const char* method, std::function<T(QVariantHash)> f, QVariantList args = QVariantList())
	{
		auto outMsg = MessageConstructor::methodCallMsg(method, args);
		_transport->sendMessage(outMsg.get());
		return _transport->getResponse(outMsg->messageID(), 5)
			.then([&f](WSResponse resp)
				{
					auto message = resp.message();
					if (resp.error() != NetworkError::NoError)
					{
						QMetaEnum metaEnum = QMetaEnum::fromType<NetworkError>();
						qDebug() << "Custom method call received an error " << resp.error();
						throw MethodCallFailure(metaEnum.valueToKey(resp.error()));
					}
					if (message->compareData("status", "error"))
						throw MethodCallFailure(message->data().value("errorString").toString());
					if (message->compareData("status", "success"))
						return f(message->data());
					else
						throw MethodCallFailure("Unknown message status received");
				});
	}
};