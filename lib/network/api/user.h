#pragma once
#include "data.h";
#include <QString>
#include "networkmanager.h"
namespace User::Api
{
	struct Get
	{
		Get() = default;
		int id = User::invalidID;
		QFuture<Data> exec(NetworkManager* handler);
	private:
		static constexpr char methodName[] = "getUser";
	};

	struct Find
	{
		Find() = default;
		std::optional<int> limit;
		bool useRegexp = false;
		std::optional<QString> name;
		std::optional<QString> tag;
		std::optional<int> id;
		QFuture<QList<int>> exec(NetworkManager* handler);
	private:
		static constexpr char methodName[] = "findUsers";
	};
	struct Update 
	{
		Update() = default;
		std::optional<QString> name;
		std::optional<QString> tag;
		//std::optional<Data::Status> status;
		QFuture<Data*> exec(NetworkManager* handler);
	private:
		static constexpr char methodName[] = "updateUser";
	};
	
}