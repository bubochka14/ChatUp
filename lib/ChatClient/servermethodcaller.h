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
class CHAT_CLIENT_EXPORT MethodCallFailure
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
	virtual QFuture<HashList> getUserRooms(int id)							= 0;
	virtual QFuture<HashList> getRoomUsers(int id)							= 0;
	virtual QFuture<HashList> getRoomHistory(int id)						= 0;
	virtual QFuture<QVariantHash> getUserInfo(int id)						= 0;
	virtual QFuture<QVariantHash> addUserToRoom(int roomID, int userID)		= 0;
	virtual QFuture<QVariantHash> createMessage(const QVariantHash&)		= 0;
	virtual QFuture<QVariantHash> createRoom(const QVariantHash&)			= 0;
	virtual QFuture<QVariantHash> deleteRoom(int id)						= 0;
	virtual QFuture<QVariantHash> updateRoom(const QVariantHash&)			= 0;
	virtual QFuture<QVariantHash> updateMessage(const QVariantHash&)		= 0;
	virtual QFuture<QVariantHash> deleteMessage(int roomId, int messageId)	= 0;
	virtual QFuture<QVariantHash> updateUser(const QVariantHash&)		    = 0;
	virtual QFuture<QVariantHash> deleteUser(int id)						= 0;

	virtual QFuture<QVariantHash> registerUser(const QString& login, const QString& pass) = 0;
	virtual QFuture<QVariantHash> loginUser(const QString& login, const QString& pass)	  = 0;
protected:
	explicit ServerMethodCaller(QObject* parent = nullptr);

};