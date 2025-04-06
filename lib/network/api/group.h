#pragma once
#include "networkcoordinator.h"
#include "data.h"
#include "core.h"
#include <QFuture>
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_GROUP_API);
namespace Group::Api
{
	struct Create 
	{
		Create() = default;
		std::string name;
		std::optional<std::string> tag;
		QFuture<ExtendedData> exec(std::shared_ptr<NetworkCoordinator> net);
		static void handle(std::shared_ptr<NetworkCoordinator> net,
			std::function<void(ExtendedData&&)> h);
	private:
		static constexpr char methodName[] = "createRoom";

	};
	struct Update 
	{
		struct GroupUpdate
		{
			std::optional<std::string> name;
			std::optional<std::string> tag;
			std::optional<int> messageCount;
			std::optional<int> foreignReadings;
			std::optional<int> localReadings;
			int id = Group::invalidID;
			void extractTo(ExtendedData& data);
		};
		
		Update() = default;
		int roomID = 0;
		std::optional<std::string> name;
		std::optional<std::string> tag;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> h);
		static void handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(GroupUpdate&&)> h);

	private:
		static constexpr char methodName[] = "updateRoom";
	};
	struct Get 
	{
		//todo
	};
	struct GetAll 
	{
		GetAll() = default;
		QFuture<std::vector<ExtendedData>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "getUserRooms";
	};
	struct AddUser
	{
		struct Desc
		{
			int roomID;
			int userID;
		};
		AddUser() = default;
		int roomID =0;
		int userID =0;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
		static void handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(Desc&&)> h);
	private:
		static constexpr char methodName[] = "addUserToRoom";
	};
	struct Delete 
	{
		Delete() = default;
		int roomID =0;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "deleteRoom";
	};
	struct GetHistory 
	{
		GetHistory() = default;
		int roomID = 0;
		QFuture<std::vector<Message::Data>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "getRoomHistory";

	};
	struct GetUsers
	{
		GetUsers() = default;
		int roomID = 0;
		QFuture<std::vector<User::Data>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "getRoomUsers";
	};
}
