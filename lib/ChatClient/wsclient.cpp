#include "wsclient.h"
#include <QtCore/QDebug>
Q_LOGGING_CATEGORY(LC_WSClient, "WebSocketClient");
WSClient::WSClient(QWebSocketProtocol::Version ver,QObject* parent)
    :QObject(parent)
    ,_webSocket(new QWebSocket(QString(),ver,this))
    ,_connected(false)
    ,_autodelete(true)
{
    connect(_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &WSClient::closed);
    connect(_webSocket, &QWebSocket::errorOccurred, this, &WSClient::errorReceived);
    connect(this, &WSClient::errorReceived, this, &WSClient::onError);
    connect(_webSocket, &QWebSocket::textMessageReceived,
        this, &WSClient::onTextMessageReceived);
}
bool WSClient::autoDeleteMessages() const
{
    return _autodelete;
}
void WSClient::setAutoDeleteMessages(bool st)
{
    _autodelete = st;
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

void WSClient::onTextMessageReceived(const QString& textMessage)
{
    QJsonObject&& obj = QJsonDocument::fromJson(textMessage.toUtf8()).object();
    qCDebug(LC_WSClient) << "Received message from " << _webSocket->requestUrl() << " : " << textMessage;
    if (obj.value("type") == "response")
    {
        QPointer<WSReply> reply(new WSReply);
        if (!reply->extractFromHash(obj.toVariantHash()))
        {
            qCWarning(LC_WSClient) << "Cannot handle reply message";
            reply.clear();
        }
        else
        {
            emit replyReceived(reply);
            if(autoDeleteMessages())
                reply.clear();
        }
    }
    else if (obj.value("type") == "methodCall")
    {
        QPointer<WSMethodCall> call(new WSMethodCall);
        qDebug() << obj.toVariantHash();
        if (!call->extractFromHash(obj.toVariantHash()))
        {
            qCWarning(LC_WSClient) << "Cannot handle methodCall message";
            call.clear();
        }
        else
        {
            emit methodCallReceived(call);
            if (autoDeleteMessages())
                call.clear();
        }
    }
    else
    {
        qCWarning(LC_WSClient) << "Unknown message type received";
    }
}
bool WSClient::sendMessage(WSMessage* message)
{
    QByteArray&& msg = QJsonDocument::fromVariant(message->toHash()).toJson();
    qCDebug(LC_WSClient).noquote() << "Sending ws message: " << msg;
    qCDebug(LC_WSClient) <<_webSocket->sendBinaryMessage(msg) << " bytes were sent.";
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