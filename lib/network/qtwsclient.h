#pragma once
#include <QtCore/QObject>
#include "transport.h"
#include <qtimer.h>
#include <QFuture>
#include <QtWebSockets/QWebSocket>
#include <qscopeguard.h>
#include "messageconstructor.h"
#include <qnetworkreply.h>
#include <QLoggingCategory>
#include <QtConcurrent/qtconcurrentrun.h>
#include "network_include.h"
#include <thread>
#include <queue>
#include "taskqueue.h"

Q_DECLARE_LOGGING_CATEGORY(LC_QtWSClient);

class CC_NETWORK_EXPORT  QtQtWSClient : public Transport
{
    Q_OBJECT;
public:
    explicit QtWSClient(QWebSocketProtocol::Version ver, QObject* parent = nullptr);
    bool autoDeleteMessages() const;
    void setAutoDeleteMessages(bool st);
    ~QtWSClient();
    bool sendMessage(WSMessage*) override;
    bool connect2Server(const QUrl& url) override;
private:
    TaskQueue _taskQueue;
    QWebSocket *_webSocket;
    QString _lastError;
    bool _autodelete;
    bool _connected;
    //MessageHandler* _msgHandler;
    QThread thr;
private slots:
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onTextMessageReceived(const QString& message);
    void onDisconnect();
};
