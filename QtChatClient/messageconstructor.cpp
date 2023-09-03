#include "messageconstructor.h"
bool WSMessage::isValid() const
{
	return _type != WSMessage::Undefined;
}
QString WSMessage::apiVersion() const
{
	return _apiVersion;
}
ulong WSMessage::messageID() const
{
	return _messageID;
}
WSMessage::MessageType WSMessage::type() const
{
	return _type;
}
QVariantHash WSMessage::data() const
{
	return _data;
}
QJsonObject WSMessage::toJsonObject() const
{
	QJsonObject jObj;
	jObj.insert("apiVersion", _apiVersion);
	jObj.insert("type", MessageConstructor::typeMapper(_type));
	jObj.insert("messageID", QJsonValue::fromVariant(QVariant::fromValue(_messageID)));
	jObj.insert("data", QJsonObject::fromVariantHash(_data));

	return jObj;
}

bool WSMessage::compareData(const QString& key, const QString& value) const
{
	if (!data().contains(key))
		return false;
	return data().value(key).toString().toUpper() == value.toUpper();
}
ulong MessageConstructor::generateID()
{
	static ulong i = 1;
	return ++i;
}
QSharedPointer<WSMessage> MessageConstructor::emptyMsg()
{
	auto out = QSharedPointer<WSMessage>(new WSMessage);
	out->_type = WSMessage::Undefined;
	return out;
}
QSharedPointer<WSMessage> MessageConstructor::responseMsg(ulong responseTo, const QVariantHash& data)
{
	auto out = QSharedPointer<WSMessage>(new WSMessage);
	out->_apiVersion = API_VERSION;
	out->_type = WSMessage::Response;
	out->_messageID = generateID();
	out->_data.insert("responseTo", QVariant::fromValue(responseTo));
	out->_data.insert(data);
	return out;
}
QSharedPointer<WSMessage> MessageConstructor::methodCallMsg(const QString& method,const QVariantList& args, const QVariantHash& data)
{
	auto out = QSharedPointer<WSMessage>(new WSMessage);
	out->_apiVersion = API_VERSION;
	out->_type = WSMessage::MethodCall;
	out->_messageID = generateID();
	out->_data.insert("method", method);
	out->_data.insert("args",args);
	out->_data.insert(data);
	return out;
}
QSharedPointer<WSMessage> MessageConstructor::fromJson(QByteArray array)
{
	auto out = QSharedPointer<WSMessage>(new WSMessage);
	QJsonDocument doc = QJsonDocument::fromJson(array);
	QJsonObject obj = doc.object();
	out->_apiVersion = obj.value("apiVersion").toString();
	out->_type = typeMapper(obj.value("type").toString());
	out->_messageID = obj.value("messageID").toInteger();
	out->_data = obj.value("data").toObject().toVariantHash();

	return out;
}
WSMessage::MessageType MessageConstructor::typeMapper(const QString& type)
{
	if (type == "authentication")
		return WSMessage::Authentication;
	else if (type == "methodCall")
		return WSMessage::MethodCall;
	else if (type == "response")
		return WSMessage::Response;
	else
		return WSMessage::Undefined;
}
QString MessageConstructor::typeMapper(WSMessage::MessageType type)
{
	if (type == WSMessage::Authentication)
		return "authentication";
	else if (type == WSMessage::MethodCall)
		return "methodCall";
	else if (type == WSMessage::Response)
		return "response";
	else
		return "undefined";
 }