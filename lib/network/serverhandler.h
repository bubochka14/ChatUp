#pragma once
#include <qobject.h>
#include <wsclient.h>
#include "network_include.h"
#include "messageconstructor.h"
#include "authenticationmaster.h"
#include <taskqueue.h>
#include <qnetworkreply.h>
#include <qqueue.h>
#include <qloggingcategory.h>
#include <rtc/channel.hpp>
Q_DECLARE_LOGGING_CATEGORY(LC_SERVER_HANDLER);
using NetworkError = QNetworkReply::NetworkError;

class CC_NETWORK_EXPORT MethodCallFailure : std::exception
{
public:
	QString message;
	NetworkError error;
};
class CC_NETWORK_EXPORT ServerHandler 
{
public:
	explicit ServerHandler(std::shared_ptr<rtc::Channel>, QObject* parent = nullptr);
	bool isConnected() const;
	void serverMethod(const char* method, QVariantHash args, QPromise<HashList>* prom);
	using Callback = std::function<void(QVariantHash&&)>;
	void addClientHandler(Callback&& h, const QString& method);
protected:
	void handleTimeout();
	void handleTextMessage(std::string);
	void handleError(std::string);
	void setIsConnected(bool);
	void handleMethodCall(WSMethodCall* call);
	void handleReply(WSReply* reply);
private:
	bool _isConnected;
	QHash<int, QPromise<HashList>*> _requests;
	QHash<QString, QList<Callback>> _clientHandlers;
	QQueue<WSMessage*> _msgQueue;
	Transport* _transport;
	//QTimer _timeoutTimer;
	QThread thread;
};