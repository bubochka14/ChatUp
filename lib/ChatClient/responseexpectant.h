#pragma once
#include "wsclient.h"
#include <QtConcurrent/qtconcurrentrun.h>
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
class ResponseExpectant
{
public:
	explicit ResponseExpectant(WSClient* ws);
    virtual QFuture<WSResponse> expectResponse(int id, size_t timeout);
private:
    WSClient* _ws;
};