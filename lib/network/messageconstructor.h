#pragma once
#include <QObject>
#include <QJsonDocument>
#include <qmetatype.h>
#include <QJsonObject>
#include "network_include.h"
#include <QMetaEnum>
class CC_NETWORK_EXPORT WSMessage : public QObject
{
	Q_OBJECT;
public:
	QString apiVersion() const;
	int messageId() const;
	void setApiVersion(const QString&);
	void setMessageId(int other);
	virtual QVariantHash toHash() const;
	virtual bool extractFromHash(const QVariantHash&);
	explicit WSMessage(QObject* parent = nullptr);
private:
	QString _api;
	int     _id;
};
class CC_NETWORK_EXPORT WSReply : public WSMessage
{
	Q_OBJECT;
public:
	enum ReplyStatus{
		unknown,
		success,
		error
	}; Q_ENUM(ReplyStatus);
	explicit WSReply(QObject* parent = nullptr);
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
class CC_NETWORK_EXPORT WSMethodCall : public WSMessage
{
public:
	explicit WSMethodCall(QObject* parent = nullptr);
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
class CC_NETWORK_EXPORT WSMessageConstructor
{
public:
	static WSReply* replyMsg(int replyTo, const QList<QVariantHash>& reply = QList<QVariantHash>());
	static WSMethodCall* methodCallMsg(const QString& method,const QVariantHash& args = QVariantHash());
	static WSMethodCall* methodCallMsg(const QString& method,QVariantHash&& args = QVariantHash());
protected:
	static void defaultMsgInstallation(WSMessage* other);
private:
	static int generateID();
};