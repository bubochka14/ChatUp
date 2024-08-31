#include "WSServerMethodCaller.h"

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

QVariant customMethod(const char* method, WSServerMethodCaller*caller,const QVariantHash& args)
{
	auto&& outMsg = MessageConstructor::methodCallMsg(method, args);
	QEventLoop loop; 
	QScopedPointer<QObject> context(new QObject);
	WSMessage wsmsg; 
	bool connectionError = false;
	QTimer timer; timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, context.get(), [=, &loop = loop]()
		{
			loop.exit();
		});
	QObject::connect(caller->transport(), &WSClient::responseReceived, context.get(),
		[&](const WSMessage& msg, size_t id) 
		{
			if (outMsg.messageID == id)
			{
				wsmsg = msg;
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
			out->message = "Connection failure";
			out->error = QNetworkReply::ConnectionRefusedError;
			throw out;
		}
		if (wsmsg.compareData("status", "error"))
		{
			MethodCallFailure out;
			out.message = wsmsg.data.value("errorString").toString().toStdString();
			out.error = QNetworkReply::InternalServerError;
			throw out;
		}
		else if (wsmsg.compareData("status", "success"))
		{

			if (!wsmsg.data.contains("return"))
				qWarning() << "Response doesn`t contain return field";
 			if (wsmsg.data.value("return").canConvert<QVariantList>())
			{
				HashList out;
				for (auto& i : wsmsg.data.value("return").toList())
				{
					out.append(i.toHash());
				}
				return QVariant::fromValue(std::move(out));
			}
			else {
				return wsmsg.data.value("return");
			}
		}
		else
		{
			MethodCallFailure* out = new MethodCallFailure;
			out->message = "Unknown methodCall failure";
			throw out;
		}
	}
	else
	{
		MethodCallFailure* out = new MethodCallFailure;
		out->message = "Timeout";
		out->error = QNetworkReply::TimeoutError;
		throw out;
	}
}
QFuture<HashList> WSServerMethodCaller::getUserRooms(int userID)
{
	return QtConcurrent::run(customMethod, "getUserRooms", this, QVariantHash({ { "id",userID } })).then([](QVariant&& v) {return qvariant_cast<HashList>(v); });
}
QFuture<QVariantHash> WSServerMethodCaller::createMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });
}
QFuture<QVariantHash> WSServerMethodCaller::createRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "createRoom", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteRoom(int id)
{
	return QtConcurrent::run(customMethod, "deleteRoo,", this, QVariantHash({ { "id",id } }))
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateRoom(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteMessage(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::updateUser(const QVariantHash& h)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, h)
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<QVariantHash> WSServerMethodCaller::deleteUser(int id)
{
	return QtConcurrent::run(customMethod, "sendChatMessage", this, QVariantHash({ { "id",id } }))
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });

}
QFuture<HashList> WSServerMethodCaller::getRoomHistory(int roomID)
{
	return QtConcurrent::run(customMethod, "getRoomHistory",this, QVariantHash({ { "roomId",roomID } })).then([](QVariant&& v) {return qvariant_cast<HashList>(v); });;
}
//QFuture<bool> WSServerMethodCaller::addUserToRoom(int userID, int roomID)
//{
//	return QtConcurrent::run(customMethod, "addUserToRoom", QVariantList() << roomID << userID);
//}
QFuture<QVariantHash> WSServerMethodCaller::registerUser(const QString& login, const QString& password)
{
	return QtConcurrent::run(customMethod, "registerUser", this, QVariantHash({ { "login",login },{"password",password}}))
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });
}
QFuture<QVariantHash> WSServerMethodCaller::loginUser(const QString& login, const QString& password)
{
	return QtConcurrent::run(customMethod, "loginUser", this, QVariantHash({ { "login",login },{"password",password} }))
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });
}
QFuture<QVariantHash> WSServerMethodCaller::getUserInfo(int id)
{
	return QtConcurrent::run(customMethod, "getUserInfo", this, QVariantHash({ { "id",id } }))
	.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });
}
QFuture<QVariantHash> WSServerMethodCaller::addUserToRoom(int roomID, int userID)
{
	return QtConcurrent::run(customMethod, "addUserToRoom", this, QVariantHash({ { "roomId",roomID },{"userId",userID} }))
		.then([](QVariant&& v) {return qvariant_cast<QVariantHash>(v); });
}
QFuture<HashList> WSServerMethodCaller::getRoomUsers(int roomID)
{
	return QtConcurrent::run(customMethod, "getRoomUsers", this, QVariantHash({ { "roomId",roomID } })).then([](QVariant&& v) {return qvariant_cast<HashList>(v); });;
}
