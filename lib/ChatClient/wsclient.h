#pragma once
#include <QtCore/QObject>
#include <qeventloop>
#include <qtimer.h>
#include <QFuture>
#include <QtWebSockets/QWebSocket>
#include <qscopeguard.h>
#include "messageconstructor.h"
#include "chatroomstructs.h"
#include <qnetworkreply.h>
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LC_WSClient);
using NetworkError = QNetworkReply::NetworkError;
class WSClient : public QObject
{
    Q_OBJECT
public:
    explicit WSClient(QWebSocketProtocol::Version ver, QObject* parent = nullptr);
    QString lastError() const;
    virtual ~WSClient();
public slots:
    bool sendMessage(WSMessage*, 
        QJsonDocument::JsonFormat = QJsonDocument::Indented);
    bool connect2Server(const QUrl& url);
signals:
    void closed();
    void errorReceived(QAbstractSocket::SocketError error);
    void responseReceived(int id, const QSharedPointer<WSMessage> resp);
    void postMessage(int roomID, const ChatRoomMessage&);
    void proxy(int responseTo, int awaitTime);
private:
    QHash<int, QFuture<WSMessage>> _expectants;
    QWebSocket *_webSocket;
    QString _lastError;
private slots:
    void onError(QAbstractSocket::SocketError error);
    void onConnected();
    void onTextMessageReceived(QString message);
    void onDisconnect();

};
