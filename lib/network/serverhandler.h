#pragma once
#include <qobject.h>
#include <wsclient.h>
#include "network_include.h"
class CC_NETWORK_EXPORT ServerHandler : public QObject
{
	Q_OBJECT;
	Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged);
public:
	explicit ServerHandler(QObject* parent = nullptr);
	bool isConnected() const;
signals:
	void isConnectedChanged();
protected:
	void setIsConnected(bool);
private:
	bool isCon;
};
class CC_NETWORK_EXPORT WSServerHandler : public  ServerHandler
{
	Q_OBJECT;
public:
	explicit WSServerHandler(WSClient* cl);
};