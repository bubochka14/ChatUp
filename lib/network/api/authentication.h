#pragma once 
#include "data.h"
#include "networkmanager.h"
namespace Api
{
	struct Login
	{
		Login() = default;
		QString login;
		QString password;
		QFuture<User::Data> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "loginUser";

	};
	struct Register
	{
		Register() =default;
		QString login;
		QString password;
		QFuture<User::Data> exec(NetworkManager* h);
	private:
		static constexpr char methodName[] = "registerUser";
	};
}