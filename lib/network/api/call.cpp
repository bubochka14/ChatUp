#include "call.h"
using namespace Call::Api;
using namespace Qt::Literals::StringLiterals;
Q_LOGGING_CATEGORY(LC_CALL_API, "CallApi");
QFuture<void> Join::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(callName, { {"roomID",roomID},{"sdp",sdp} })
	.then([](json&& res) {
		});
}

void Join::handle(std::shared_ptr<NetworkCoordinator> h,
	std::function<void(Participate::Data&&)> cb)
{
	h->addClientHandler(callName,[handler = std::move(cb)](json&& in) {
		handler(Participate::Data(std::move(in)));
	});
}
QFuture<void> Disconnect::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(callName, { {"roomID",roomID} }).then([](json&&) {

		});
}
void Disconnect::handle(std::shared_ptr<NetworkCoordinator> h,
	std::function<void(Participate::Data&&)> cb)
{
	h->addClientHandler(callName, [handler = std::move(cb)](json&& in) {
		handler(Participate::Data(std::move(in)));
		});
}
void UpdateCallMedia::handle(std::shared_ptr<NetworkCoordinator> h,
	std::function<void(MediaUpdate&&)> cb)
{
	h->addClientHandler(callName, [handler = std::move(cb)](json&& in) {
		try {
			MediaUpdate media;
			media.audio = in["audio"];
			media.video = in["video"];
			media.roomID= in["roomID"];
			media.userID = in["userID"];
			handler(std::move(media));
		}
		catch (json::exception& e)
		{
			qCWarning(LC_CALL_API) << "Call media parsing error:" << e.what();
		}
		});
}
QFuture<void> UpdateCallMedia::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json out;
	if(video.has_value())
		out["video"] = video.value();
	if (audio.has_value())
		out["audio"] = audio.value();

	return h->serverMethod(callName, out).then([](json&&) {

		});
}