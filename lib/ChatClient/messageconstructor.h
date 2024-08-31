#pragma once
#include <QObject>
#include <QJsonDocument>
#include <qmetatype.h>
#include <QJsonObject>
class WSMessage
{
	Q_GADGET
public:
	enum MessageType
	{
		Undefined =0,
		Response,
		MethodCall,

	}; Q_ENUM(MessageType);
	explicit WSMessage();
	QString apiVersion;
	int	messageID;
	MessageType type;
	QVariantHash data;
	//Case insensitive compare.
	bool compareData(const QString& key, const QString& value) const;
	bool isValid() const;
	QJsonObject toJsonObject() const;
};
class MessageConstructor
{
public:
	static WSMessage responseMsg(int responseTo, const QVariantHash& data = QVariantHash());
	static WSMessage methodCallMsg(const QString& method, const QVariantHash& args = QVariantHash());
	static WSMessage fromJson(QByteArray array);
	static WSMessage::MessageType typeMapper(const QString& type);
	static QString typeMapper(WSMessage::MessageType);
private:
	static int generateID();
};