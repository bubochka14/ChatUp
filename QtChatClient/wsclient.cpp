#include "wsclient.h"
#include <QtCore/QDebug>
Q_LOGGING_CATEGORY(LC_MessageExpectant, "MessageExpectant");

Q_LOGGING_CATEGORY(LC_WSClient, "WebSocketClient");
WSResponse::WSResponse(QNetworkReply::NetworkError st,
    QSharedPointer<WSMessage> mess)
    :_error(st)
    ,_message(mess)
{

}
NetworkError WSResponse::error() const
{
    return _error;
}

QSharedPointer<WSMessage> WSResponse::message() const
{
    return _message;
}
//FutureResponse::FutureResponse(WSClient* wsCLient,int responseTo)
//    :_wsCLient(wsCLient)
//    ,_status(Expected)
//{
//    _message = MessageConstructor::responseMsg(responseTo);
//    if (!_wsCLient->getResponse(responseTo).isNull())
//    {
//        _message = _wsCLient->getResponse(responseTo);
//        _status = Received;
//        emit received();
//    }
//    else
//    {
//        connect(_wsCLient, &WSClient::responseReceived,this, [=](int rt)
//            {
//                if (rt == responseTo)
//
//                {
//                    _message = _wsCLient->getResponse(rt);
//                    _status = Received;
//                    emit received();
//                }
//            });
//    }
//
//}
//bool FutureResponse::isReceived() const
//{
//    return _isReceived;
//}
//QString  FutureResponse::errorString() const
//{
//    return _errorString;
//}
//FutureResponse::Status FutureResponse::status() const
//{
//    return _status;
//}
//FutureResponse::Status FutureResponse::await(int sec)
//{
//    QTimer timer;
//    timer.setSingleShot(true);
//    timer.start(sec * 1000);
//    QEventLoop eLoop;
//    auto cleanUp = qScopeGuard([&] { disconnect(this, 0, 0, 0); });
//    if (_status != Expected)
//        return _status;
//    connect(this, &FutureResponse::received,this, [&](){eLoop.quit(); return  _status; });
//    connect(&timer, &QTimer::timeout, [&]() {
//        setStatus(FutureResponse::Timeout); 
//        eLoop.quit();
//        return _status; 
//        });
//    connect(_wsCLient, &WSClient::errorReceived,this, [&](QAbstractSocket::SocketError error) {
//        setStatus(FutureResponse::NetworkError);
//        _errorString = _wsCLient->lastError();
//        eLoop.quit();
//        return _status;
//        });
//    eLoop.exec();
//
//}
//void FutureResponse::setStatus(Status st)
//{
//    _status = st;
//}
//QSharedPointer<WSMessage> FutureResponse::message() const
//{
//    return _message;
//}
WSClient::WSClient(QWebSocketProtocol::Version ver,QObject* parent)
    :QObject(parent)
    ,_webSocket(new QWebSocket(QString(),ver,this))
{
    qCDebug(LC_WSClient) << "WebSocket created";
    connect(_webSocket, &QWebSocket::connected, this, &WSClient::onConnected);
    connect(_webSocket, &QWebSocket::disconnected, this, &WSClient::closed);
    connect(_webSocket, &QWebSocket::errorOccurred, this, &WSClient::errorReceived);
    connect(this, &WSClient::errorReceived, this, &WSClient::onError);

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
    connect(_webSocket, &QWebSocket::textMessageReceived,
        this, &WSClient::onTextMessageReceived);
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
        if (_serverResponses.contains(responseTo))
        {
            qCWarning(LC_WSClient) << "Re-response to " << responseTo << " received, it will be ignored. ";
            return;
        }
        qCDebug(LC_WSClient) << "Received response to " << responseTo << " message from " << _webSocket->requestUrl();

        _serverResponses.insert(recMess->data().value("responseTo").toUInt(), recMess);
        emit responseReceived(recMess->data().value("responseTo").toUInt());
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
QFuture<WSResponse>  WSClient::getResponse(int responseTo,int awaitTime)
{
    QPromise<WSResponse> promise;
    //promise.create();
    QFuture<WSResponse> future = promise.future();
    promise.start();
    if (_serverResponses.contains(responseTo))
    {
        promise.addResult(WSResponse(QNetworkReply::NoError, _serverResponses[responseTo]));
        promise.finish();
        return future;
    }
    MessageExpectant* e = new MessageExpectant;
    e->moveToThread(&_thread);
    connect(e, &MessageExpectant::received, this, [&, p = std::move(promise)]() mutable {
        p.addResult(WSResponse(QNetworkReply::NoError, _serverResponses[responseTo]));
        p.finish();
        _thread.quit();
        sender()->deleteLater();
        //delete promise;
        }, Qt::SingleShotConnection);
    connect(e, &MessageExpectant::timeout,this, [&, p = std::move(promise)]() mutable {
        p.addResult(WSResponse(QNetworkReply::TimeoutError));
        p.finish();
        _thread.quit();
        sender()->deleteLater();
        //delete promise;
        }, Qt::SingleShotConnection);
    _thread.start();
    e->expectResponseTo(this, responseTo, awaitTime);
    return future;
}
void WSClient::onError(QAbstractSocket::SocketError error)
{
    _lastError = _webSocket->errorString();
    qCDebug(LC_WSClient) << "WebSocket error: " << _lastError;
}
QString WSClient::lastError() const
{
    return _lastError;
}
MessageExpectant::~MessageExpectant()
{
    qDebug(LC_MessageExpectant) << "deleted";
}
MessageExpectant::MessageExpectant()
{
    qDebug(LC_MessageExpectant) << "created";
}
void MessageExpectant::expectResponseTo(WSClient* ws, int id, int time)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    auto con1 = connect(ws, &WSClient::responseReceived, [&](int id_)
        {
            if (id == id_)
            {
                loop.exit();
                qDebug(LC_MessageExpectant) << "Expected response received";
                emit received();
            }
        });
    auto con2 = connect(&timer, &QTimer::timeout, [&]()
        {
            loop.exit();
            qDebug(LC_MessageExpectant) << "TimerEnd";
            emit timeout();
           

        });
    timer.start(time * 1000);
    loop.exec();
    disconnect(con1);
    disconnect(con2);
}

void WSClient::onDisconnect()
{
    qCDebug(LC_WSClient) << "Socket disconected ";
}
