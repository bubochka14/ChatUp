#pragma once
#include "data.h"
#include <QString>
#include <QFuture>
#include "networkcoordinator.h"
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LC_USER_API);
namespace User::Api
{
	struct Get
	{
		Get() = default;
		std::optional<int> id;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> coordinator);
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
		QFuture<std::vector<Data>> exec(std::shared_ptr<NetworkCoordinator> coordinator);
	private:
		static constexpr char methodName[] = "findUsers";
	};
	struct Update 
	{
		struct UserUpdate{
			std::optional<std::string> name;
			std::optional<std::string> tag;
			std::optional<Status> status;
			int id;
		};
		Update() = default;
		std::optional<std::string> name;
		std::optional<std::string> tag;
		QFuture<Data> exec(std::shared_ptr<NetworkCoordinator> handler);
		static void handle(std::shared_ptr<NetworkCoordinator> coord, std::function<void(UserUpdate)>);
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
