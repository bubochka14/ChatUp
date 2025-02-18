#pragma once
#include "data.h"
#include "networkmanager.h"
#include <string>
#include <qfuture>
namespace Message::Api
{
	struct Create 
	{
		Create() =default;
		std::string body;
		int roomID = 0;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> h);
		static void handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(Data&&)> h);
	private:
		static constexpr char methodName[] = "sendChatMessage";

	};
	struct Find
	{
		Find() = default;
		std::optional<std::string> messageIndex;
		std::optional<std::string> id;
		std::optional<std::string> userID;
		int roomID = 0;
		QFuture<std::vector<Data>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "findMessage";
	};
	struct Update
	{
		Update() = default;
		int messageID = 0;
		std::string body;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "updateMessage";
	};
	struct GetByIndex 
	{
		GetByIndex() = default;
		int roomID = 0;
		int startIndex = 0;
		int endIndex = 0;
		QFuture<std::vector<Data>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "getMessagesByIndex";
	};
	struct Delete
	{
		Delete();
		int messageID = 0;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "deleteMessage";
	};
	
}
