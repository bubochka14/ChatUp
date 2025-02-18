#include "sdp.h"
Q_LOGGING_CATEGORY(LC_SDP_API, "RtcApi")
using namespace rtc;

QFuture<void> rtc::Api::Description::exec(std::shared_ptr<NetworkCoordinator> coord)
{
	return coord->serverMethod(std::string(method),
		{ 
			{"id",id },
			{"description",std::move(description)}, 
			{"type",std::move(type)}
		}, NetworkCoordinator::DirectCall)
		.then([](json&& res) {});
}
void rtc::Api::Description::handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(Data::Description&&)> h)
{
	net->addClientHandler(method, [handler = std::move(h)](json&& res) {
		Data::Description out;
		try
		{
			res["id"].get_to(out.id);
			res["description"].get_to(out.description);
			res["type"].get_to(out.type);
		}
		catch (const json::exception& e)
		{
			qCCritical(LC_SDP_API) << "RTC Description parsing error:" << e.what();
		}
		handler(std::move(out));
		});
}
QFuture<void> rtc::Api::Candidate::exec(std::shared_ptr<NetworkCoordinator> coord)
{
	return coord->serverMethod(std::string(method),
		{
			{"id",id },
			{"candidate",std::move(candidate)},
			{"mid",std::move(mid)}
		}, NetworkCoordinator::DirectCall)
		.then([](json&& res) {});
}
void rtc::Api::Candidate::handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(Data::Candidate&&)> h)
{
	net->addClientHandler(method, [handler = std::move(h)](json&& res) {
		Data::Candidate out;
		try
		{
			res["id"].get_to(out.id);
			res["mid"].get_to(out.mid);
			res["candidate"].get_to(out.candidate);
		}
		catch (const json::exception& e)
		{
			qCCritical(LC_SDP_API) << "RTC Candidate parsing error:" << e.what();
		}
		handler(std::move(out));
		});
}

