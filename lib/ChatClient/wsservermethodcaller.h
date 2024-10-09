#pragma once
#include "servermethodcaller.h"
#include "wsclient.h"
#include "chatclient_include.h"
#include <qpointer.h>
class CHAT_CLIENT_EXPORT WSServerMethodCaller : public ServerMethodCaller
{
	Q_OBJECT;
public:
	explicit  WSServerMethodCaller(WSClient* trasnport, const QString& hostname, int port, QObject* parent = nullptr);
	WSClient* transport() const;
	QUrl serverUrl() const;
	QFuture<HashList>     getUserRooms(const QVariantHash&) override;
	QFuture<HashList>	  getRoomUsers(const QVariantHash&) override;
	QFuture<HashList>	  getRoomHistory(const QVariantHash&) override;
	QFuture<QVariantHash> getUsers(const QVariantHash&) override;
	QFuture<HashList>	  findUsers(const QVariantHash&) override;
	QFuture<QVariantHash> addUserToRoom(const QVariantHash&) override;
	QFuture<QVariantHash> createRoom(const QVariantHash&) override;
	QFuture<QVariantHash> deleteRoom(const QVariantHash&) override;
	QFuture<QVariantHash> updateRoom(const QVariantHash&);
	QFuture<QVariantHash> createMessage(const QVariantHash&) override;
	QFuture<QVariantHash> updateMessage(const QVariantHash&) override;
	QFuture<QVariantHash> deleteMessage(const QVariantHash&) override;
	QFuture<QVariantHash> updateUser(const QVariantHash&) override;
	QFuture<QVariantHash> deleteUser(const QVariantHash&) override;
	QFuture<QVariantHash> registerUser(const QVariantHash&) override;
	QFuture<QVariantHash> loginUser(const QVariantHash&) override;
	QFuture<QVariantHash> setReadMessagesCount(const QVariantHash&) override;
	QFuture<QVariantHash> getReadMessagesCount(const QVariantHash&) override;
signals:
	void connectionAttempt(const QUrl&);
	void transferMessage(WSMessage* msg);
private:
	WSClient* _transport;
	QUrl  _server;
};