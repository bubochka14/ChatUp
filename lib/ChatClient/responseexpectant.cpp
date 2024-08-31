//#include "responseexpectant.h"
//WSResponse::WSResponse(QNetworkReply::NetworkError st, const WSMessage& mess)
//    :_error(st)
//    , _message(mess)
//{}
//NetworkError WSResponse::error() const
//{
//    return _error;
//}
//void WSResponse::setMessage(const  WSMessage& other)
//{
//    _message = other;
//}
//void WSResponse::setError(NetworkError other)
//{
//    _error = other;
//}
//WSMessage WSResponse::message() const
//{
//    return _message;
//}
////ResponseExpectant::ResponseExpectant(WSClient* ws)
////    :_ws(ws)
////{
////}
////WSResponse _expect(WSClient* ws,int id, size_t timeout)
////{
////    QEventLoop loop; QScopedPointer<QObject> context(new QObject);
////    WSResponse out;
////    QTimer::singleShot(timeout, context.get(), [&]()
////        {
////            out = WSResponse(QNetworkReply::TimeoutError);
////            loop.exit();
////        }
////    );
////    QObject::connect(ws, &WSClient::responseReceived,
////        context.get(), [&](int responseTo, QSharedPointer<WSMessage> msg)
////        {
////            if (responseTo == id)
////            {
////                out = WSResponse(QNetworkReply::NoError, msg);
////                loop.exit();
////            }
////        });
////    loop.exec();
////    return out;
////}
////QFuture<WSResponse> ResponseExpectant::expectResponse(int id, size_t timeout)
////{
////    return QtConcurrent::run(_expect,_ws,id, timeout);
////}
