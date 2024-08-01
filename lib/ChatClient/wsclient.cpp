#include "wsclient.h"
#include <QtCore/QDebug>
Q_LOGGING_CATEGORY(LC_WSClient, "WebSocketClient");
WSClient::WSClient(QWebSocketProtocol::Version ver,QObject* parent)
    :QObject(parent)
    ,_webSocket(new QWebSocket(QString(),ver,this))
{
    connect(_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &WSClient::closed);
    connect(_webSocket, &QWebSocket::errorOccurred, this, &WSClient::errorReceived);
    connect(this, &WSClient::errorReceived, this, &WSClient::onError);
    connect(_webSocket, &QWebSocket::textMessageReceived,
        this, &WSClient::onTextMessageReceived);
    qCDebug(LC_WSClient) << "WebSocket created";

}

bool WSClient::connect2Server(const QUrl& url)
{
    _webSocket->open(QUrl(url));
    return true;
}
void WSClient::onConnected()
{
    qCDebug(LC_WSClient) << "Request " << _webSocket->request().url();
    qCDebug(LC_WSClient) << "WebSocket connected";
}

void WSClient::onTextMessageReceived(QString textMessage)
{
    auto recMess = MessageConstructor::fromJson(textMessage.toUtf8());
    qCDebug(LC_WSClient) << "Received message from " << _webSocket->requestUrl() << " : " << textMessage;

    switch (recMess->type())
    {
    case WSMessage::Response :
    {
        if (!recMess->data().value("responseTo").isValid())
        {
            qCCritical(LC_WSClient) << "Cannot find 'responseTo' field in response message from server";
            return;
        }
        int responseTo = recMess->data().value("responseTo").toInt();
        qCDebug(LC_WSClient) << "Received response to " << responseTo << " message from " << _webSocket->requestUrl();
        emit responseReceived(recMess->data().value("responseTo").toUInt(),recMess );
    }
    break;
    case WSMessage::MethodCall :
    {
        QHash rhash = recMess->data().value("args").toList().at(1).toHash();
        ChatRoomMessage mess(UserInfo(rhash.value("user").toHash().value("name").toString(), rhash.value("user").toHash().value("id").toInt()),
            rhash.value("time").toDateTime(), rhash.value("body").toByteArray());
        emit postMessage(recMess->data().value("args").toList().first().toInt(), mess);
    }
    break;
    default:
        qCCritical(LC_WSClient) << "Unknown message type received";
        break;
    }
}
bool WSClient::sendMessage(WSMessage * message, QJsonDocument::JsonFormat format)
{
    QJsonDocument doc(message->toJsonObject());
    auto jDoc = doc.toJson(format);
    qCDebug(LC_WSClient).noquote() << "Sending ws message: " << QString(jDoc.toStdString().c_str());
    qCDebug(LC_WSClient) <<_webSocket->sendBinaryMessage(jDoc) << " bytes were sent.";
    return true;
}
void WSClient::onError(QAbstractSocket::SocketError error)
{
    _lastError = _webSocket->errorString();
    qCDebug(LC_WSClient) << "WebSocket error: " << _lastError;
}
WSClient::~WSClient()
{
}
QString WSClient::lastError() const
{
    return _lastError;
}
void WSClient::onDisconnect()
{
    qCDebug(LC_WSClient) << "Socket disconnected ";
}
