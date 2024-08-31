#include "messageconstructor.h"
bool WSMessage::isValid() const
{
	return type != WSMessage::Undefined;
}
WSMessage::WSMessage()
	:type(Undefined)
	,messageID(0)
{

}
QJsonObject WSMessage::toJsonObject() const
{
	QJsonObject jObj;
	jObj.insert("apiVersion", apiVersion);
	jObj.insert("type", MessageConstructor::typeMapper(type));
	jObj.insert("messageID", QJsonValue::fromVariant(QVariant::fromValue(messageID)));
	jObj.insert("data", QJsonObject::fromVariantHash(data));

	return jObj;
}

bool WSMessage::compareData(const QString& key, const QString& value) const
{
	if (!data.contains(key))
		return false;
	return data.value(key).toString().toUpper() == value.toUpper();
}
int MessageConstructor::generateID()
{
	static int i = 1;
	return ++i;
}
WSMessage MessageConstructor::responseMsg(int responseTo, const QVariantHash& data)
{
	WSMessage out;
	//out->_apiVersion = API_VERSION;
	out.type = WSMessage::Response;
	out.messageID = generateID();
	out.data.insert("responseTo", QVariant::fromValue(responseTo));
	out.data.insert(data);
	return out;
}
WSMessage MessageConstructor::methodCallMsg(const QString& method, const QVariantHash& args)
{
	WSMessage out;
	//out->_apiVersion = API_VERSION;
	out.type = WSMessage::MethodCall;
	out.messageID = generateID();
	out.data.insert("method", method);
	out.data.insert("args",args);
	return out;
}
WSMessage MessageConstructor::fromJson(QByteArray array)
{
	WSMessage out;
	QJsonDocument doc = QJsonDocument::fromJson(array);
	QJsonObject obj = doc.object();
	out.apiVersion = obj.value("apiVersion").toString();
	out.type = typeMapper(obj.value("type").toString());
	out.messageID = obj.value("messageID").toInteger();
	out.data = obj.value("data").toObject().toVariantHash();

	return out;
}
WSMessage::MessageType MessageConstructor::typeMapper(const QString& type)
{
	if (type == "methodCall")
		return WSMessage::MethodCall;
	else if (type == "response")
		return WSMessage::Response;
	else
		return WSMessage::Undefined;
}
QString MessageConstructor::typeMapper(WSMessage::MessageType type)
{
	if (type == WSMessage::MethodCall)
		return "methodCall";
	else if (type == WSMessage::Response)
		return "response";
	else
		return "undefined";
 }