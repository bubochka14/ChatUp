#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#define API_VERSION "1.0"
class WSMessage
{
	Q_GADGET
public:
	enum MessageType
	{
		Undefined =0,
		Authentication,
		Response,
		MethodCall,

	};
	Q_ENUM(MessageType);
private:
	WSMessage() = default;
	QString _apiVersion;
	ulong	_messageID = 0;
	MessageType _type = Undefined;
	QVariantHash _data;
public:
	QString apiVersion() const;
	ulong messageID() const;
	MessageType type() const;
	QVariantHash data() const;
	//Case insensitive compare.
	bool compareData(const QString& key, const QString& value) const;
	bool isValid() const;
	QJsonObject toJsonObject() const;
	friend class MessageConstructor;
};
class MessageConstructor
{
public:
	static QSharedPointer<WSMessage> responseMsg(ulong responseTo, const QVariantHash& data = QVariantHash());
	static QSharedPointer<WSMessage> methodCallMsg(const QString& method, const QVariantList & args=QVariantList(),
		const QVariantHash& data = QVariantHash());
	static QSharedPointer<WSMessage> emptyMsg();
	static QSharedPointer<WSMessage> fromJson(QByteArray array);
	static WSMessage::MessageType typeMapper(const QString& type);
	static QString typeMapper(WSMessage::MessageType);
private:
	static ulong generateID();
};