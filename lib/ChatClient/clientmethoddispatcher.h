#pragma once
#include <qobject.h>
#include "wsclient.h"
#include "chatclient_include.h"
#include <qloggingcategory.h>
class CHAT_CLIENT_EXPORT ClientMethodDispatcher : public QObject
{
	Q_OBJECT;
public:
	using Handler = std::function<void(const QVariantHash&)>;
	explicit ClientMethodDispatcher(QObject* parent = nullptr);
	virtual ~ClientMethodDispatcher() = default;
public slots:
	virtual void addCustomHandler(const QString& method, Handler handler) = 0;
signals:
	void messagePosted(const QVariantHash& data);
	void updatedUser(const QVariantHash& data);
	void updatedRoom(const QVariantHash& data);
	void updatedMessage(const QVariantHash& data);
	void deletedMessage(const QVariantHash& data);
	void deletedRoom(const QVariantHash& data);
	void disconnected(const QVariantHash& data);

};
Q_DECLARE_LOGGING_CATEGORY(LC_WS_DISPATCHER);
class WSClientMethodDispatcher : public ClientMethodDispatcher
{
	Q_OBJECT;
public:
	explicit WSClientMethodDispatcher(WSClient* client, QObject* parent = nullptr);
public slots:
	void handleMethodCall(WSMethodCall* call);
	void addCustomHandler(const QString& method, Handler handler) override;

private:
	QMap<QString, QList<Handler>> _handlers;
	WSClient* _ws;
};