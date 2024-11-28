#pragma once
#include <qobject.h>
#include "wsclient.h"
#include "network_include.h"
#include <qloggingcategory.h>
class CC_NETWORK_EXPORT ClientMethodDispatcher : public QObject
{
	Q_OBJECT;
public:
	using Handler = std::function<void(const QVariantHash&)>;
	explicit ClientMethodDispatcher(QObject* parent = nullptr);
	virtual ~ClientMethodDispatcher() = default;
public slots:
	virtual void addHandler(const QString& method, Handler handler) = 0;
signals:
	void disconnected(const QVariantHash& data);

};
Q_DECLARE_LOGGING_CATEGORY(LC_WS_DISPATCHER);
class CC_NETWORK_EXPORT WSClientMethodDispatcher : public ClientMethodDispatcher
{
	Q_OBJECT;
public:
	explicit WSClientMethodDispatcher(WSClient* client, QObject* parent = nullptr);
public slots:
	void handleMethodCall(WSMethodCall* call);
	void addHandler(const QString& method, Handler handler) override;

private:
	QMap<QString, QList<Handler>> _handlers;
	WSClient* _ws;
};