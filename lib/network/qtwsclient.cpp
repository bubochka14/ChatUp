#include "qtwsclient.h"
#include <QtCore/QDebug>
Q_LOGGING_CATEGORY(LC_QtWSClient, "WebSocketClient");
QtWSClient::QtWSClient(QWebSocketProtocol::Version ver,QObject* parent)
    :Transport(parent)
    ,_webSocket(nullptr)
    ,_connected(false)
    ,_autodelete(true)
    //,_msgHandler(new MessageHandler(this))
{
    _webSocket = new QWebSocket(QString(), ver);
    _webSocket->moveToThread(&thr);
    connect(_webSocket, &QWebSocket::connected, this, &QtWSClient::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &QtWSClient::closed);
    connect(_webSocket, &QWebSocket::errorOccurred, this, &QtWSClient::onError);
    connect(_webSocket, &QWebSocket::textMessageReceived, this, &QtWSClient::onTextMessageReceived);
    thr.start();
}
bool QtWSClient::autoDeleteMessages() const
{
    return _autodelete;
}
void QtWSClient::setAutoDeleteMessages(bool st)
{
    _autodelete = st;
}
bool QtWSClient::connect2Server(const QUrl& url)
{
    if (isConnected())`
        return true;
    _webSocket->open(QUrl(url));
 //   qCDebug(LC_QtWSClient) << "Request " << _webSocket->request().url();
}
void QtWSClient::onConnected()
{
    _connected = true;
    qCDebug(LC_QtWSClient) << "WebSocket connected";
    emit connected();

}
//MessageHandler::MessageHandler(QtWSClient* parent)
//{
//    connect(this, &MessageHandler::replyReceived, parent, &QtWSClient::replyReceived/*, Qt::DirectConnection*/);
//    connect(this, &MessageHandler::methodCallReceived, parent, &QtWSClient::methodCallReceived/*, Qt::DirectConnection*/);
//}

void QtWSClient::onTextMessageReceived(const QString& textMessage)
{
    QJsonObject&& obj = QJsonDocument::fromJson(textMessage.toUtf8()).object();
    if (obj.value("type") == "response")
    {
        WSReply* reply = new WSReply;
        if (!reply->extractFromHash(obj.toVariantHash()))
        {
            qCWarning(LC_QtWSClient) << "Cannot handle reply message";
            //reply->deleteLater();
        }
        else
        {
            emit replyReceived(reply);
            //if(autoDeleteMessages())
            //    reply->deleteLater();
        }
    }
    else if (obj.value("type") == "methodCall")
    {
        WSMethodCall* call = new WSMethodCall;
        qDebug() << obj.toVariantHash();
        if (!call->extractFromHash(obj.toVariantHash()))
        {
            qCWarning(LC_QtWSClient) << "Cannot handle methodCall message";
            //call->deleteLater();
        }
        else
        {
            emit methodCallReceived(call);
            //if (autoDeleteMessages())
            //    call->deleteLater();
        }
    }
    else
    {
        qCWarning(LC_QtWSClient) << "Unknown message type received";
    }
}
bool QtWSClient::sendMessage(WSMessage* message)
{
    QByteArray&& msg = QJsonDocument::fromVariant(message->toHash()).toJson();
 
  // qCDebug(LC_QtWSClient).noquote() << "Sending ws message: " << msg;
    _taskQueue.enqueue([this,msg]() {
        _webSocket->sendBinaryMessage(msg);
        });
    return true;
}
void QtWSClient::onError(QAbstractSocket::SocketError error)
{
    _lastError = _webSocket->errorString();
    if (error == QAbstractSocket::RemoteHostClosedError)
        _connected = false;
}
QtWSClient::~QtWSClient()
{
}
void QtWSClient::onDisconnect()
{
    _connected = false;
    qCDebug(LC_QtWSClient) << "Socket disconnected ";
}
