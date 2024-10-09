#include "WSServerMethodCaller.h"
using namespace Qt::Literals::StringLiterals;
WSServerMethodCaller::WSServerMethodCaller(WSClient* transport, const QString& host,int port,QObject* parent)
	:ServerMethodCaller(parent)
	,_transport(transport)
{
	using namespace Qt::Literals::StringLiterals;
	_server.setHost(host);
	_server.setPort(port);
	_server.setScheme(u"ws"_s);

	connect(this, &WSServerMethodCaller::connectionAttempt, _transport, &WSClient::connect2Server);
	connect(this, &WSServerMethodCaller::transferMessage, _transport, &WSClient::sendMessage);
}
WSClient* WSServerMethodCaller::transport() const
{
	return _transport;
}
QUrl WSServerMethodCaller::serverUrl() const
{
	return _server;
}

HashList customMethod(const char* method, WSServerMethodCaller*caller,const QVariantHash& args)
{
	WSMethodCall* outMsg = WSMessageConstructor::methodCallMsg(method, args);
	QEventLoop loop; 
	QScopedPointer<QObject> context(new QObject);
	QPointer<WSReply> wsrep;
	bool connectionError = false;
	QTimer timer; timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, context.get(), [=, &loop = loop]()
		{
			loop.exit();
		});
	QObject::connect(caller->transport(), &WSClient::replyReceived, context.get(),
		[&](WSReply* rep) 
		{
			if (outMsg->messageId() == rep->replyTo())
			{
				wsrep = rep;
				loop.exit();
			}
		});
	QObject::connect(caller->transport(), &WSClient::errorReceived, context.get(),
		[&](QAbstractSocket::SocketError error) 
		{
			connectionError = true;
			loop.exit();
		},Qt::SingleShotConnection);
	timer.start(caller->timeout());
	if (!caller->transport()->isConnected())
	{
		QObject::connect(caller->transport(), &WSClient::connected, context.get(), [=]() {
			caller->transferMessage(outMsg);
			});
		caller->connectionAttempt(caller->serverUrl());
	}else
		caller->transferMessage(outMsg);
	loop.exec();
	if (timer.isActive())
	{
		if (connectionError)
		{
			MethodCallFailure* out = new MethodCallFailure;
			out->message = "Connection failure"_L1;
			out->error = QNetworkReply::ConnectionRefusedError;
			throw out;
		}
		if (wsrep->status() == WSReply::error)
		{
			MethodCallFailure out;
			out.message = wsrep->errorString();
			out.error = QNetworkReply::InternalServerError;
			throw out;
		}
		else if (wsrep->status() == WSReply::success)
		{
 			return wsrep->reply();
		}
		else
		{
			MethodCallFailure* out = new MethodCallFailure;
			out->message = "Unknown methodCall failure"_L1;
			throw out;
		}
	}
	else
	{
		MethodCallFailure* out = new MethodCallFailure;
		out->message = "Timeout"_L1;
		out->error = QNetworkReply::TimeoutError;
		throw out;
	}
}
QFuture<HashList> WSServerMethodCaller::getUserRooms(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "getUserRooms", this, h);
}
QFuture<QVariantHash> WSServerMethodCaller::createMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });
}
QFuture<QVariantHash> WSServerMethodCaller::createRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "createRoom", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "deleteRoom", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "updateRoom", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "updateMessage", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "deleteMessage", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateUser(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "updateUser", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteUser(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "deleteUser", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<HashList> WSServerMethodCaller::getRoomHistory(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "getRoomHistory", this, h);
}
QFuture<QVariantHash> WSServerMethodCaller::registerUser(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "registerUser", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });
}
QFuture<QVariantHash> WSServerMethodCaller::loginUser(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "loginUser", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });
}
QFuture<QVariantHash> WSServerMethodCaller::getUsers(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "getUsers", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });
}
QFuture<HashList> WSServerMethodCaller::findUsers(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "findUsers", this, h);
}
QFuture<QVariantHash> WSServerMethodCaller::addUserToRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "addUserToRoom", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });;
}
QFuture<HashList> WSServerMethodCaller::getRoomUsers(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "getRoomUsers", this, h);
}
QFuture<QVariantHash> WSServerMethodCaller::setReadMessagesCount(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "setReadMessagesCount", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });

}
QFuture<QVariantHash> WSServerMethodCaller::getReadMessagesCount(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "getReadMessagesCount", this, h)
		.then([](HashList&& v) {return v.takeFirst(); });
}