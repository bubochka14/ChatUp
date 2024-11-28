#include "MessageConstructor.h"
using namespace Qt::Literals::StringLiterals;
WSMessage::WSMessage(QObject* parent)
	:QObject(parent)
	,_id(-1)

{
}
QString WSMessage::apiVersion() const
{
	return _api;
}
int WSMessage::messageId() const
{
	return _id;
}
void WSMessage::setApiVersion(const QString& other)
{
	_api = other;
}
bool  WSMessage::extractFromHash(const QVariantHash& hash)
{
	if (hash.contains("apiVersion"_L1))
		_api = hash["apiVersion"_L1].toString();
	else
		return false;
	if (hash.contains("messageID"_L1))
		_id = hash["messageID"_L1].toInt();
	else
		return false;
	return true;
}

void WSMessage::setMessageId(int other)
{
	_id = other;
}
QVariantHash WSMessage::toHash() const
{
	return QVariantHash({
		{"apiVersion"_L1,_api},
		{"messageID"_L1,_id}
		});
}
WSReply::WSReply(QObject* parent)
	:WSMessage(parent)
	,_repTo(-1)
	,_status(unknown)
{}
int WSReply::replyTo() const
{
	return _repTo;
}
WSReply::ReplyStatus WSReply::status() const
{
	return _status;
}
QList<QVariantHash> WSReply::reply() const
{
	return _reply;
}
void WSReply::setReplyTo(int other)
{
	_repTo = other;
}
void WSReply::setStatus(ReplyStatus other)
{
	_status = other;
}
void WSReply::setReply(const QList<QVariantHash>& other)
{
	_reply = other;
}
void WSReply::setReply(QList<QVariantHash>&& other)
{
	_reply = std::move(other);
}
WSMethodCall::WSMethodCall(QObject* parent)
	:WSMessage(parent)
{}
QString WSMethodCall::method() const
{
	return _method;
}
QVariantHash WSMethodCall::args() const
{
	return _args;
}
void WSMethodCall::setArgs(const QVariantHash& other)
{
	_args = other;
}
void WSMethodCall::setArgs(QVariantHash&& other)
{
	_args = std::move(other);
}
void WSMethodCall::setMethod(const QString& other)
{
	_method = other;
}
bool WSMethodCall::extractFromHash(const QVariantHash& hash)
{
	WSMessage::extractFromHash(hash);
	if (!hash.contains("data"_L1))
		return false;
	QVariantHash&& data = hash["data"_L1].toHash();
	if (data.contains("method"_L1)&& data["method"].canConvert<QString>())
		_method = data["method"_L1].toString();
	else
		return false;
	if (data.contains("args"_L1))
		_args = data["args"_L1].toHash();
	return true;
}
QVariantHash WSMethodCall::toHash() const
{
	QVariantHash&& hash = WSMessage::toHash();
	hash.insert("data"_L1,QVariantHash{
		{"method"_L1,_method},
		{"args"_L1,_args}
	});
	hash.insert("type"_L1, "methodCall"_L1);

	return hash;
}
QString WSReply::errorString() const
{
	return _error;
}
void WSReply::setErrorString(const QString& other)
{
	_error = other;
}
bool WSReply::extractFromHash(const QVariantHash& hash)
{
	WSMessage::extractFromHash(hash);
	if (!hash.contains("data"_L1))
		return false;
	QVariantHash&& data = hash["data"_L1].toHash();
	if (data.contains("responseTo"_L1))
		_repTo = data["responseTo"_L1].toInt();
	else
		return false;
	if (data.contains("status"_L1))
	{
		_status = ReplyStatus(QMetaEnum::fromType<WSReply::ReplyStatus>()
			.keyToValue(data["status"_L1].toString().toStdString().c_str()));
	}
	else
		return false;
	if (data.contains("return"_L1))
	{
		_reply.clear();
		if(data["return"_L1].canConvert<QVariantList>()) {
			for (auto& i : data["return"_L1].toList())
			{
				_reply.emplaceBack(i.toHash());
			}
		}
		else _reply.emplaceBack(data["return"_L1].toHash());
	}
	if (data.contains("errorString"_L1))
		_error = data["errorString"_L1].toString();

	return true;
}
QVariantHash WSReply::toHash() const
{
	QVariantHash&& hash = WSMessage::toHash();
	hash.insert("data"_L1,QVariantHash({
		{"responseTo"_L1,_repTo},
		{"return"_L1,QVariant::fromValue(_reply)},
		{"status"_L1,QMetaEnum::fromType<WSReply::ReplyStatus>().valueToKey(_status)},
		{"errorString"_L1,_error},
		}));
	hash.insert("type"_L1, "response"_L1);
	return hash;
}
int WSMessageConstructor::generateID()
{
	static int i = 1;
	return ++i;
}
WSReply* WSMessageConstructor::replyMsg(int replyTo, const QList<QVariantHash>& data)
{

	WSReply* out = new WSReply();
	defaultMsgInstallation(out);
	out->setReplyTo(replyTo);
	out->setReply(data);
	return out;
}
void WSMessageConstructor::defaultMsgInstallation(WSMessage* other)
{
	other->setApiVersion("1.0.0"_L1);
	other->setMessageId(generateID());
}
WSMethodCall* WSMessageConstructor::methodCallMsg(const QString& method, QVariantHash&& args)
{
	WSMethodCall* out = new WSMethodCall();
	defaultMsgInstallation(out);
	out->setMethod(method);
	out->setArgs(std::move(args));
	return out;
}

WSMethodCall* WSMessageConstructor::methodCallMsg(const QString& method, const QVariantHash& args)
{
	WSMethodCall* out = new WSMethodCall();
	defaultMsgInstallation(out);
	out->setMethod(method);
	out->setArgs(args);
	return out;
}

