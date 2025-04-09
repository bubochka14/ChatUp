#pragma once
#include <qstring.h>
#include "data.h"
#include "networkcoordinator.h"
#include <QFuture>
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_CALL_API);
namespace Call::Api
{

	struct Join
	{
		Join() = default;
		int roomID = Group::invalidID;
		std::string sdp;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
		static void handle(std::shared_ptr<NetworkCoordinator> h, std::function<void(Participate::Data&&)> cb);
	private:
		static constexpr char callName[] = "joinCall";

	};
	struct Disconnect
	{
		Disconnect() = default;
		int roomID = Group::invalidID;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
		static void handle(std::shared_ptr<NetworkCoordinator> h, std::function<void(Participate::Data&&)> cb);
	private:
		static constexpr char callName[] = "disconnectCall";
	};
	struct Get
	{
		Get() = default;
		int roomID = Group::invalidID;
		QFuture<std::vector<Participate::Data>> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char callName[] = "getCall";
	};
	struct UpdateCallMedia
	{
		struct MediaUpdate
		{
			bool video;
			bool audio;
			int userID;
			int roomID;
		};
		UpdateCallMedia() = default;
		std::optional<bool> audio;
		std::optional<bool> video;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
		//Cb args: room id and user id
		static void handle(std::shared_ptr<NetworkCoordinator> h, std::function<void(MediaUpdate&&)> cb);
	private:
		static constexpr char callName[] = "updateCallMedia";
	};

}