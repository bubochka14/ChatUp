#pragma once
#include "network_include.h"
#include <string>
#include <qfuture.h>
#include <networkcoordinator.h>
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_SDP_API);
namespace rtc::Data
{
	static constexpr int invalidID = 0;
	struct Description
	{
		int id = invalidID;
		std::string description;
		std::string type;
	};
	struct Candidate
	{
		int id = invalidID;
		std::string candidate;
		std::string mid;
	};
}
namespace rtc::Api
{
	class CC_NETWORK_EXPORT Description
	{
	public:
		Description() = default;
		int id = Data::invalidID;
		std::string description;
		std::string type;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> coord);
		static void handle(std::shared_ptr<NetworkCoordinator> net,
			std::function<void(Data::Description&&)> h);

	private:
		static constexpr char method[] = "RtcDescription";

	};
	class CC_NETWORK_EXPORT Candidate
	{
	public:
		Candidate() = default;
		int id = Data::invalidID;
		std::string mid;
		std::string candidate;
		QFuture<void> exec(std::shared_ptr<NetworkCoordinator> coord);
		static void handle(std::shared_ptr<NetworkCoordinator> net,
			std::function<void(Data::Candidate&&)> h);

	private:
		static constexpr char method[] = "RtcCandidate";

	};
}