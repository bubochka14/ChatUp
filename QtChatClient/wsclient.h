#pragma once
#include <QtCore/QObject>
#include <qeventloop>
#include <qtimer.h>
#include <QFuture>
#include <QtWebSockets/QWebSocket>
#include <qscopeguard.h>
#include "messageconstructor.h"
#include "chatroomstructs.h"
#include "singletonholder.h"
#include <qnetworkreply.h>
#include <QtConcurrent>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LC_WSClient);
Q_DECLARE_LOGGING_CATEGORY(LC_MessageExpectant);
using NetworkError = QNetworkReply::NetworkError;
class WSClient;
struct WSResponse
{
    explicit WSResponse(NetworkError st = NetworkError::UnknownServerError,
        QSharedPointer<WSMessage> mess = MessageConstructor::emptyMsg());
public:
    NetworkError error() const;
    QSharedPointer<WSMessage> message() const;
private:
    NetworkError _error;
    QSharedPointer<WSMessage> _message;
};
class WSClient : public QObject
{
    Q_OBJECT
public:
    explicit WSClient(QWebSocketProtocol::Version ver, QObject* parent = nullptr);
    QFuture<WSResponse> getResponse(int responseTo, int awaitTime);
    QString lastError() const;
public slots:
    bool sendMessage(WSMessage*, 
        QJsonDocument::JsonFormat = QJsonDocument::Indented);
    bool connect2Server(const QUrl& url);
signals:
    void closed();
    void errorReceived(QAbstractSocket::SocketError error);
    void responseReceived(int id);
    void postMessage(int roomID, const ChatRoomMessage&);

private:
    QMap<int, QSharedPointer<WSMessage>> _serverResponses;
    QWebSocket *_webSocket;
    QString _lastError;
    QThread _thread;
private slots:
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onTextMessageReceived(QString message);
    void onDisconnect();

};
class MessageExpectant : public QObject
{
    Q_OBJECT;
public:
    explicit MessageExpectant();
    virtual ~MessageExpectant();
public slots:
    void expectResponseTo(WSClient* ws, int id, int time);
signals:
    void received();
    void timeout();
};