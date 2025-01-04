#pragma once
#include "data.h"
#include "networkmanager.h"
namespace Message::Api
{
	struct Create 
	{
		Create() =default;
		QString body;
		int roomID = 0;
		QFuture<Data> exec(NetworkManager* h);
		static void handle(NetworkManager* net, std::function<void(Data&&)> h);
	private:
		static constexpr char methodName[] = "sendChatMessage";

	};
	struct Find
	{
		Find() = default;
		std::optional<QString> messageIndex;
		std::optional<QString> id;
		std::optional<QString> userID;
		int roomID = 0;
		QFuture<QList<Data>> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "findMessage";
	};
	struct Update
	{
		Update() = default;
		int messageID = 0;
		std::optional<QString> body;
		QFuture<Data> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "updateMessage";
	};
	struct GetByIndex 
	{
		GetByIndex() = default;
		int roomID = 0;
		int startIndex = 0;
		int endIndex = 0;
		QFuture<QList<Data>> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "getMessagesByIndex";
	};
	struct Delete
	{
		Delete();
		int messageID = 0;
		QFuture<void> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "deleteMessage";
	};
	
}
