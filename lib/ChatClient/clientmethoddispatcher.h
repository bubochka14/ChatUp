#pragma once
#include <qobject.h>
#include "wsclient.h"
class ClientMethodDispatcher : public QObject
{
	Q_OBJECT;
public:
	explicit ClientMethodDispatcher(QObject* parent = nullptr);
	virtual ~ClientMethodDispatcher() = default;
signals:
	void postChatMessage(const QVariantHash& message);
public slots:
	void handleMethodCall(const QVariantHash& methodData);
};