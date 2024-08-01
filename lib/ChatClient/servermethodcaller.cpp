#include "servermethodcaller.h"

ServerMethodCaller::ServerMethodCaller(WSClient* transport, QObject* parent)
	:QObject(parent)
	,_transport(transport)
	,_exp(transport)
{
}
QFuture<RoomList>ServerMethodCaller::getUserRooms(int userID)
{
	return customMethod<RoomList>("getUserRooms", [](QVariantHash data)
		{
			RoomList list;
			QVariantHash rhash;
			for (auto& i : data.value("rooms").toList())
			{
				rhash = i.toHash();
				list << ChatRoom(rhash.value("NAME").toString(), rhash.value("ID").toInt());
			}
			return list;

		}, QVariantList() << QString::number(userID));
}
		

MethodCallFailure::MethodCallFailure(const QString& message)
	:_message(message)
	,_stdMessage(message.toStdString())
{

}
const char* MethodCallFailure::what() const noexcept 
{
	return _stdMessage.c_str();
}
QString MethodCallFailure::errorString() const noexcept
{
	return _message;
}
//QFuture<QList<UserInfo>> ServerMethodCaller::getRoomUsers(const QString& userToken, int roomID)
//{
//	auto outMsg = MessageConstructor::methodCallMsg("getRoomUsers", QVariantList(roomID), QVariantHash({ {"userToken",userToken} }));
//	return _transport->sendMessage(outMsg.get());
//}
//QFuture<QList<ChatRoomMessage>> ServerMethodCaller::getRoomHistory(const QString& userToken, int roomID)
//{
//	auto outMsg = MessageConstructor::methodCallMsg("getRoomHistory", QVariantList(roomID), QVariantHash({ {"userToken",userToken} }));
//	return _transport->sendMessage(outMsg.get());
//}
//QFuture<UserInfo> ServerMethodCaller::getUserInfo(const QString& userID)
//{
//	auto outMsg = MessageConstructor::methodCallMsg("getUserInfo", QVariantList({userID}));
//	return _transport->sendMessage(outMsg.get());
//}
//QFuture<QStringList> ServerMethodCaller::getAvailableServerMethods()
//{
//	auto outMsg = MessageConstructor::methodCallMsg("getAvailableServerMethods");
//	return _transport->sendMessage(outMsg.get());
//}
QFuture<bool> ServerMethodCaller::sendChatMessage(const QString& userToken, int roomID, const QString& message)
{
	return customMethod<bool>("sendChatMessage", [](QVariantHash data) {return true; }, QVariantList() << userToken << QString::number(roomID) << message);

}

QFuture<QList<ChatRoomMessage>> ServerMethodCaller::getRoomHistory(const QString& userToken, int roomID)
{
	return customMethod<QList<ChatRoomMessage>>("getRoomHistory", [](QVariantHash data) {
		QList<ChatRoomMessage> list;
		QVariantHash rhash;
		for (auto& i : data.value("messages").toList())
		{
			rhash = i.toHash();
			list << ChatRoomMessage(UserInfo(rhash.value("user").toHash().value("name").toString(), rhash.value("user").toHash().value("id").toInt()),
				rhash.value("time").toDateTime(), rhash.value("body").toByteArray());
		}
		return list;

		}, QVariantList() << userToken << QString::number(roomID));


}
//QFuture<bool> ServerMethodCaller::addUserToRoom(const QStrin\g& userToken, int userID, int roomID)
//{
//	auto outMsg = MessageConstructor::methodCallMsg("addUserToRoom", QVariantList()<< userToken<< roomID << userID);
//	return _transport->sendMessage(outMsg.get());
//}
QFuture<QString>ServerMethodCaller::registerUser(const UserCredentials& cr)
{
	return customMethod<QString>("registerUser", [](QVariantHash data) {return data.value("userToken").toString(); }, QVariantList() << cr.username << cr.password);
}
QFuture<QString> ServerMethodCaller::loginUser(const UserCredentials& cr)
{
	return customMethod<QString>("loginUser", [](QVariantHash data) {
		return data.value("userToken").toString(); }
	, QVariantList() << cr.username << cr.password);
}
QFuture<UserInfo> ServerMethodCaller::getUserInfo(const QString& username)
{
	return customMethod<UserInfo>("getUserInfo", [](QVariantHash data)
		{
			return UserInfo(data.value("username").toString(),
				data.value("id").toInt());
		},QVariantList() << username);
}
QFuture<UserInfo> ServerMethodCaller::getUserInfo(int id)
{
	return customMethod<UserInfo>("getUserInfoById", [](QVariantHash data)
		{
			return UserInfo(data.value("username").toString(),
			data.value("userID").toInt());
		}, QVariantList() << QString::number(id));
}
QFuture<UserInfo> ServerMethodCaller::getCurrentUserInfo(const QString& userToken)
{
	return customMethod<UserInfo>("getCurrentUserInfo", [](QVariantHash data)
		{
			return UserInfo(data.value("username").toString(),
			data.value("id").toInt());
		}, QVariantList() <<userToken);
}