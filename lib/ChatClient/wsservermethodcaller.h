#pragma once
#include "servermethodcaller.h"
#include "wsclient.h"
class WSServerMethodCaller : public ServerMethodCaller
{
	Q_OBJECT;
public:
	explicit  WSServerMethodCaller(WSClient* trasnport,const QString& hostname,int port, QObject* parent = nullptr);
	WSClient* transport() const;
	QUrl serverUrl() const;
	QFuture<HashList> getUserRooms(int userID) override;
	QFuture<HashList> getRoomUsers(int roomID) override;
	QFuture<HashList> getRoomHistory(int roomID) override;
	QFuture<QVariantHash> getUserInfo(int id) override;
	QFuture<QVariantHash> addUserToRoom(int roomID, int usserID) override;
	QFuture<QVariantHash> createRoom(const QVariantHash&) override;
	QFuture<QVariantHash> deleteRoom(int id) override;
	QFuture<QVariantHash> updateRoom(const QVariantHash&);
	QFuture<QVariantHash> createMessage(const QVariantHash&) override;
	QFuture<QVariantHash> updateMessage(const QVariantHash&) override;
	QFuture<QVariantHash> deleteMessage(const QVariantHash&) override;
	QFuture<QVariantHash> updateUser(const QVariantHash&) override;
	QFuture<QVariantHash> deleteUser(int id) override;
	QFuture<QVariantHash> registerUser(const QString& login, const QString& pass) override;
	QFuture<QVariantHash> loginUser(const QString& login, const QString& pass) override;
signals:
	void connectionAttempt(const QUrl&);
	void transferMessage(const WSMessage& msg);
private:
	WSClient* _transport;
	QUrl  _server;
};