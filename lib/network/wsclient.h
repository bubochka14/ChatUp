#pragma once
#include <QtCore/QObject>
#include <qeventloop>
#include <qtimer.h>
#include <QFuture>
#include <QtWebSockets/QWebSocket>
#include <qscopeguard.h>
#include "messageconstructor.h"
#include <qnetworkreply.h>
#include <QLoggingCategory>
#include <QtConcurrent/qtconcurrentrun.h>
#include "network_include.h"
Q_DECLARE_LOGGING_CATEGORY(LC_WSClient);
class CC_NETWORK_EXPORT  WSClient : public QObject
{
    Q_OBJECT;
public:
    explicit WSClient(QWebSocketProtocol::Version ver, QObject* parent = nullptr);
    QString lastError() const;
    bool isConnected() const;
    bool autoDeleteMessages() const;
    void setAutoDeleteMessages(bool st);
    virtual ~WSClient();
public slots:
    bool sendMessage(WSMessage*);
    bool connect2Server(const QUrl& url);
signals:
    void closed();
    void connected();
    void errorReceived(QAbstractSocket::SocketError error);
    void replyReceived(WSReply* rep);
    void methodCallReceived(WSMethodCall* call);
    //void postMessage(int roomID, const ChatRoomMessage&);
private:
    QWebSocket *_webSocket;
    QString _lastError;
    bool _autodelete;
    bool _connected;
private slots:
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onTextMessageReceived(const QString& message);
    void onDisconnect();
};
