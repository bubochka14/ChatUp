#pragma once
#include "data.h";
#include <QString>
#include <QFuture>
#include "networkmanager.h"
namespace User::Api
{
	struct Get
	{
		Get() = default;
		std::optional<int> id;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> handler);
	private:
		static constexpr char methodName[] = "getUser";
	};

	struct Find
	{
		Find() = default;
		std::optional<int> limit;
		bool useRegexp = false;
		std::optional<std::string> name;
		std::optional<std::string> tag;
		std::optional<std::string> id;
		void extractFromQHash(const QVariantHash& h);
		QFuture<std::vector<Data>> exec(std::shared_ptr<NetworkCoordinator> handler);
	private:
		static constexpr char methodName[] = "findUsers";
	};
	struct Update 
	{
		Update() = default;
		std::optional<std::string> name;
		std::optional<std::string> tag;
		//std::optional<Data::Status> status;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> handler);
	private:
		static constexpr char methodName[] = "updateUser";
	};
	//register
	struct Create
	{
		Create() = default;
		std::string login;
		std::string password;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> h);
	private:
		static constexpr char methodName[] = "registerUser";
	};
}