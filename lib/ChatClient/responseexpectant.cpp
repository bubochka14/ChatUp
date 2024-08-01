#include "responseexpectant.h"
WSResponse::WSResponse(QNetworkReply::NetworkError st,
    QSharedPointer<WSMessage> mess)
    :_error(st)
    , _message(mess)
{}
NetworkError WSResponse::error() const
{
    return _error;
}

QSharedPointer<WSMessage> WSResponse::message() const
{
    return _message;
}
ResponseExpectant::ResponseExpectant(WSClient* ws)
    :_ws(ws)
{
}
void _expect(QPromise<WSResponse>& promise, WSClient* ws,int id, size_t timeout)
{
    QEventLoop loop;
    QTimer::singleShot(timeout, [&]()
        {
            promise.addResult(WSResponse(QNetworkReply::TimeoutError));
            promise.finish();
        }
    );
    QMetaObject::Connection con = QObject::connect(ws, &WSClient::responseReceived,
        [&](int responseTo, QSharedPointer<WSMessage> msg)
        {
            if (responseTo == id)
            {
                QObject::disconnect(con);
                promise.addResult(WSResponse(QNetworkReply::NoError, msg));
                promise.finish();
            }
        });
    loop.exec();
}
QFuture<WSResponse> ResponseExpectant::expectResponse(int id, size_t timeout)
{
    return QtConcurrent::run(_expect,_ws,id, timeout);
}
