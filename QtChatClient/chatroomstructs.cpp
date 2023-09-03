#include "chatroomstructs.h"
bool UserInfo::operator==(UserInfo const& t)
{
	return name == t.name && id == t.id;
}
bool operator==(UserInfo const& t1, UserInfo const& t2)
{
	return t1.name == t2.name && t1.id == t2.id;

}
bool UserInfo::operator!=(UserInfo const& t)
{
	return name != t.name || id != t.id;
}
ChatRoom::ChatRoom(const QString& name, int roomid)
	:name(name)
	,roomID(roomid)
{

}
ChatRoomMessage::ChatRoomMessage(UserInfo from, const QDateTime& when, const QByteArray& what, ChatMessageType type)
	:user(from)
	,time(when)
	,body(what)
	,type(type)
{

}
UserInfo::UserInfo(const QString& username, int userid)
	:name(username)
	,id(userid)
{}