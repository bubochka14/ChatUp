//#pragma once
//#include "servermethodcaller.h"
//class OfflineServerMethodCaller : public ServerMethodCaller
//{
//public:
//	QFuture<QVariantHash> getUserRooms(int userID) override;
//	QFuture<QVariantHash> getRoomUsers(int roomID) override;
//	QFuture<QVariantHash> getRoomHistory(int roomID) override;
//	QFuture<QVariantHash> getUserInfo(int id) override;
//	QFuture<QVariantHash> getCurrentUserInfo() override;
//	QFuture<QVariantHash> addUserToRoom(int roomID, int usserID) override;
//	QFuture<QVariantHash> sendChatMessage(int roomID, const QString& message) override;
//	QFuture<QVariantHash> registerUser(const QString& login, const QString& pass) override;
//	QFuture<QVariantHash> loginUser(const QString& login, const QString& pass) override;
//};