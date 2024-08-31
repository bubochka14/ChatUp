#include "wsclient.h"
#include <QtCore/QDebug>
Q_LOGGING_CATEGORY(LC_WSClient, "WebSocketClient");
WSClient::WSClient(QWebSocketProtocol::Version ver,QObject* parent)
    :QObject(parent)
    ,_webSocket(new QWebSocket(QString(),ver,this))
    ,_connected(false)
{
    connect(_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &WSClient::closed);
    connect(_webSocket, &QWebSocket::errorOccurred, this, &WSClient::errorReceived);
    connect(this, &WSClient::errorReceived, this, &WSClient::onError);
    connect(_webSocket, &QWebSocket::textMessageReceived,
        this, &WSClient::onTextMessageReceived);
}

bool WSClient::connect2Server(const QUrl& url)
{
    if (isConnected())
        return true;
    _webSocket->open(QUrl(url));
    qCDebug(LC_WSClient) << "Request " << _webSocket->request().url();
}
void WSClient::onConnected()
{
    _connected = true;
    qCDebug(LC_WSClient) << "WebSocket connected";
    emit connected();

}

void WSClient::onTextMessageReceived(QString textMessage)
{
    auto recMess = MessageConstructor::fromJson(textMessage.toUtf8());
    qCDebug(LC_WSClient) << "Received message from " << _webSocket->requestUrl() << " : " << textMessage;

    switch (recMess.type)
    {
    case WSMessage::Response :
    {
        if (!recMess.data.value("responseTo").isValid())
        {
            qCCritical(LC_WSClient) << "Cannot find 'responseTo' field in response message from server";
            return;
        }
        int responseTo = recMess.data.value("responseTo").toInt();
        qCDebug(LC_WSClient) << "Received response to " << responseTo << " message from " << _webSocket->requestUrl();
        emit responseReceived(recMess,recMess.data.value("responseTo").toUInt() );
    }
    break;
    case WSMessage::MethodCall :
    {
        //QHash rhash = recMess.data.value("args").toList().at(1).toHash();
        //ChatRoomMessage mess(UserInfo(rhash.value("user").toHash()),
        //    rhash.value("time").toDateTime(), rhash.value("body").toByteArray());
        //emit postMessage(recMess.data.value("args").toList().first().toInt(), mess);
    }
    break;
    default:
        qCCritical(LC_WSClient) << "Unknown message type received";
        break;
    }
}
bool WSClient::sendMessage(const WSMessage&  message)
{
    QJsonDocument doc(message.toJsonObject());
    auto jDoc = doc.toJson(QJsonDocument::Indented);
    qCDebug(LC_WSClient).noquote() << "Sending ws message: " << QString(jDoc.toStdString().c_str());
    qCDebug(LC_WSClient) <<_webSocket->sendBinaryMessage(jDoc) << " bytes were sent.";
    return true;
}
void WSClient::onError(QAbstractSocket::SocketError error)
{
    _lastError = _webSocket->errorString();
    if (error == QAbstractSocket::RemoteHostClosedError)
        _connected = false;
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
    _connected = false;
    qCDebug(LC_WSClient) << "Socket disconnected ";
}
bool WSClient::isConnected() const
{
    return _connected;
}