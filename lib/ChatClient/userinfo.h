#pragma once
#include <QObject>
#include <QDateTime>
#include <QQmlEngine>
#include <QMetaEnum>
#include "chatclient_include.h"
class CHAT_CLIENT_EXPORT UserInfo : public QObject
{
	Q_OBJECT;
	QML_ELEMENT;
public:
	enum Status
	{
		Offline,
		Online,
	}; Q_ENUM(Status);
	explicit UserInfo(QObject *parent = nullptr);
	int id() const;
	QString name() const;
	void setId(int other);
	void setName(const QString& other);
	Status status() const;
	void setStatus(Status other);
	QVariantHash toHash() const;
	static int checkId(const QVariantHash& data, bool& st)
	{
		if (data.contains("id"))
		{
			st = true;
			return data["id"].toInt();
		}
		st = false;
		return 0;
	}
public slots:
	void extractFromHash(const QVariantHash& other);
signals:
	void idChanged();
	void nameChanged();
	void statusChanged();
private:
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged);
	Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged);
	Status _status;
	QString _name;
	int _id;
};

//class ChatRoom : public QObject
//{
//	Q_OBJECT;
//	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
//	Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged);
//	Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged);
//public:
//	explicit ChatRoom(QObject* parent = nullptr);
//	QString name() const;
//	QString tag() const;
//	int id() const;
//	void setName(const QString& other);
//	void setTag(const QString& other);
//	void setId(int other);
//	QVariantHash toHash() const;
//public slots:
//	void extractFromHash(const QVariantHash& other);
//signals:
//	void nameChanged();
//	void idChanged();
//	void tagChanged();
//private:
//	int _id;
//	QString _name;
//	QString _tag;
//};
//using RoomList = QList<ChatRoom*>;
//using UserList = QList<UserInfo*>;
//using MessageList = QList<ChatRoomMessage*>;
