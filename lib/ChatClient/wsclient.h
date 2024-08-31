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
Q_DECLARE_LOGGING_CATEGORY(LC_WSClient);
class WSClient : public QObject
{
    Q_OBJECT;
public:
    explicit WSClient(QWebSocketProtocol::Version ver, QObject* parent = nullptr);
    QString lastError() const;
    bool isConnected() const;
    virtual ~WSClient();
public slots:
    bool sendMessage(const WSMessage&);
    bool connect2Server(const QUrl& url);
signals:
    void closed();
    void connected();
    void errorReceived(QAbstractSocket::SocketError error);
    void responseReceived(const WSMessage& resp,size_t id);
    //void postMessage(int roomID, const ChatRoomMessage&);
private:
    QHash<int, QFuture<WSMessage>> _expectants;
    QWebSocket *_webSocket;
    QString _lastError;
    bool _connected;
private slots:
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onTextMessageReceived(QString message);
    void onDisconnect();

};
