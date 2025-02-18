#include "call.h"
using namespace Call::Api;
using namespace Qt::Literals::StringLiterals;
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
	h->addClientHandler(callName,[handler = std::move(cb)](json&& in) {
		handler(Participate::Data(std::move(in)));
		});
}