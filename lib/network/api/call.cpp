#include "call.h"
using namespace Call::Api;
using namespace Qt::Literals::StringLiterals;
QFuture<void> Join::exec(NetworkManager* h)
{
	return h->serverMethod(callName, { {"roomID"_L1,roomID} })
	.then([](HashList&& res) {
		});
}

void Join::handle(NetworkManager* h, std::function<void(Request&&)> cb)
{
	h->addClientHandler([handler = std::move(cb)](QVariantHash&& in) {
		Request out;
		out.roomID = in.contains("roomID"_L1)? in.take("roomID"_L1).toInt()
			:Group::invalidID;
		out.userID = in.contains("userID"_L1)? in.take("userID"_L1).toInt()
			:User::invalidID;
		handler(std::move(out));
		}, callName);
}
QFuture<void> Disconnect::exec(NetworkManager*h)
{
	return h->serverMethod(callName, { {"roomID"_L1,roomID} }).then([](HashList&&) {

		});
}
void Disconnect::handle(NetworkManager* h, std::function<void(Request&&)> cb)
{
	h->addClientHandler([handler = std::move(cb)](QVariantHash&& in) {
		Request out;
		out.roomID = in.contains("roomID"_L1) ? in.take("roomID"_L1).toInt()
			: Group::invalidID;
		out.userID = in.contains("userID"_L1) ? in.take("userID"_L1).toInt()
			: User::invalidID;
		handler(std::move(out));
		}, callName);
}