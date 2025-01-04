#pragma once
#include <QObject>
#include <QJsonDocument>
#include <qmetatype.h>
#include <QJsonObject>
#include "network_include.h"
#include <unordered_map>
#include <nlohmann/json.hpp>
using HashList = QList<QVariantHash>;
class CC_NETWORK_EXPORT RPCMessage
{
public:
	int messageId() const;
	void setApiVersion(const QString&);
	void setMessageId(int other);
	virtual json toHash() const;
	virtual bool extractFromHash(const QVariantHash&);
	explicit RPCMessage();
private:
	int     _id;
};
class CC_NETWORK_EXPORT RPCReply : public RPCMessage
{
public:
	enum ReplyStatus{
		unknown,
		success,
		error
	}; 
	explicit RPCReply();
	int replyTo() const;
	ReplyStatus status() const;
	QList<QVariantHash> reply() const;
	QString errorString() const;
	void setErrorString(const QString& other);
	void setReplyTo(int other);
	void setStatus(ReplyStatus other);
	void setReply(const QList<QVariantHash>& other);
	void setReply(QList<QVariantHash>&& other);
	bool extractFromHash(const QVariantHash&) override;
	QVariantHash toHash() const override;

private:
	int _repTo;
	QString _error;
	ReplyStatus _status;
	QList<QVariantHash> _reply;
};
class CC_NETWORK_EXPORT RPCMethodCall : public RPCMessage
{
public:
	explicit RPCMethodCall();
	QString method() const;
	QVariantHash args() const;
	void setArgs(const QVariantHash& other);
	void setArgs(QVariantHash&& other);
	void setMethod(const QString& other);
	QVariantHash toHash() const override;
	bool extractFromHash(const QVariantHash&) override;

private:
	QString _method;
	QVariantHash _args;
};
class CC_NETWORK_EXPORT RPCMessageConstructor
{
public:
	static RPCReply*      replyMsg(int replyTo, const QList<QVariantHash>& reply = QList<QVariantHash>());
	static RPCMethodCall* methodCallMsg(std::string method,const QVariantHash& args = QVariantHash());
	static RPCMethodCall* methodCallMsg(std::string method, std::unordered_map args);
protected:
	static void defaultMsgInstallation(RPCMessage* other);
private:
	static int generateID();
};