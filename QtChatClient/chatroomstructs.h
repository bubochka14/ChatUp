#pragma once
#include <QObject>
#include <QDateTime>

struct UserInfo
{
	Q_GADGET;
	Q_PROPERTY(QString name MEMBER name);
	Q_PROPERTY(int id MEMBER id);
public:
	explicit UserInfo(const QString& username = "", int userid = 0);
	QString name;
	int id;
	bool operator==(UserInfo const&);
	bool operator!=(UserInfo const&);

	friend bool operator==(UserInfo const&, UserInfo const&);
};

using UserList = QList<UserInfo>;

struct ChatRoomMessage
{
	Q_GADGET;

public:
	enum ChatMessageType
	{
		Utf8_Text,
		Utf16_Text,
		Base64_Image
	}; Q_ENUM(ChatMessageType);
	explicit ChatRoomMessage(UserInfo from, const QDateTime& when,
		const QByteArray& what, ChatMessageType type = Utf16_Text);

	UserInfo  user;
	QDateTime time;
	QByteArray body;
	ChatMessageType type;
private:
	Q_PROPERTY(UserInfo user MEMBER user);
	Q_PROPERTY(QDateTime time MEMBER time);
	Q_PROPERTY(QByteArray body MEMBER body);
	Q_PROPERTY(ChatMessageType type MEMBER type);
};
using MessageList = QList<ChatRoomMessage>;
struct ChatRoom
{
	Q_GADGET;
public:
	explicit ChatRoom(const QString& name = "Unknown", int roomid = 0);
	QString name;
	int roomID;
	MessageList messages;
	UserList users;

};
using RoomList = QList<ChatRoom>;
