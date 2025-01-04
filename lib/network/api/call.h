#pragma once
#include <qstring.h>
#include "data.h"
#include "networkmanager.h"
#include <qfuture>
namespace Call::Api
{
	struct Request
	{
		int roomID = Group::invalidID;
		int	userID = User::invalidID;
	};

	struct Join
	{
		Join() = default;
		int roomID = Group::invalidID;
		QFuture<void> exec(NetworkManager* h);
		static void handle(NetworkManager* h, std::function<void(Request&&)> cb);
	private:
		static constexpr char callName[] = "joinCall";

	};
	struct Disconnect
	{
		Disconnect() = default;
		int roomID = Group::invalidID;
		QFuture<void> exec(NetworkManager* h);
		static void handle(NetworkManager* h, std::function<void(Request&&)> cb);
	private:
		static constexpr char callName[] = "disconnectCall";


	};
}