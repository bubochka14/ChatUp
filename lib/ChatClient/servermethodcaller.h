#pragma once
#include <qobject.h>
#include "wsclient.h"
#include "messageconstructor.h"
#include <QNetworkReply>
#include <QMetaEnum>
#include <functional>
#include <QFuture>
using NetworkError = QNetworkReply::NetworkError;
using HashList = QList<QVariantHash>;
class CHAT_CLIENT_EXPORT MethodCallFailure : std::exception
{
public:
	QString message;
	NetworkError error;
};
class CHAT_CLIENT_EXPORT ServerMethodCaller : public QObject
{
	Q_OBJECT;
	size_t _timeout;
public:
	void setTimeout(size_t);
	size_t timeout() const;

	virtual QFuture<HashList>	  getUserRooms	(const QVariantHash& data) = 0;
	virtual QFuture<HashList>	  getRoomUsers	(const QVariantHash& data) = 0;
	virtual QFuture<HashList>	  getRoomHistory(const QVariantHash& data) = 0;
	virtual QFuture<HashList>	  findUsers		(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> getUsers		(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> addUserToRoom (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> createMessage (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> createRoom    (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> deleteRoom    (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> updateRoom    (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> updateMessage (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> deleteMessage (const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> updateUser	(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> deleteUser	(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> registerUser	(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> loginUser		(const QVariantHash& data) = 0;

	virtual QFuture<QVariantHash> setReadMessagesCount(const QVariantHash& data) = 0;
	virtual QFuture<QVariantHash> getReadMessagesCount(const QVariantHash& data) = 0;
protected:
	explicit ServerMethodCaller(QObject* parent = nullptr);

};