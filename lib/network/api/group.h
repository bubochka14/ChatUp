#pragma once
#include "networkmanager.h"
#include "data.h"
namespace Group::Api
{
	struct Create 
	{
		Create() = default;
		QString name;
		std::optional<QString> tag;
		QFuture<Data> exec(NetworkManager* h);
		//void handle(NetworkManager* h)
	private:
		static constexpr char methodName[] = "createRoom";

	};
	struct Update 
	{
		Update() = default;
		int roomID = 0;
		std::optional<QString> name;
		std::optional<QString> tag;
		QFuture<Data> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "updateRoom";
	};
	struct Get 
	{

	};
	struct GetAll 
	{
		GetAll() = default;
		QFuture<QList<Data>> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "getUserRooms";
	};
	struct AddUser 
	{
		AddUser() = default;
		int roomID =0;
		int userID =0;
		QFuture<void> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "addUserToRoom";
	};
	struct Delete 
	{
		Delete() = default;
		int roomID =0;
		QFuture<void> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "deleteRoom";
	};
	struct GetHistory 
	{
		GetHistory() = default;
		int roomID = 0;
		QFuture<QList<Message::Data>> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "getRoomHistory";

	};
	struct MarkRead 
	{
		MarkRead() = default;
		size_t count=0;
		int roomID=0;
		QFuture<void> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "setReadMessagesCount";

	};
}
