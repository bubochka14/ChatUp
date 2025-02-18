#pragma once
#include <qstring.h>
#include "data.h"
#include "networkmanager.h"
#include <qfuture>
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
}