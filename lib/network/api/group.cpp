#pragma once
#include "group.h"
using namespace Group::Api;
void Create::handle(std::shared_ptr<NetworkCoordinator> net,
	std::function<void(ExtendedData&&)> h)
{
	static constexpr char methodName[] = "addRoom";
	net->addClientHandler(methodName,[handler = std::move(h)](json&& res) {
		handler(Group::ExtendedData(std::move(res)));
		});
}

QFuture<Group::ExtendedData> Create::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args = {
		{"name",std::move(name)},
		{"type","group"}
	};
	if (tag.has_value())
		args.emplace("tag", *std::move(tag));
	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		return Group::ExtendedData(res);
	});
}
QFuture<std::vector<Group::ExtendedData>> GetAll::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName,{ {"type","group"} }).then([](json&& res) {
		std::vector<Group::ExtendedData> out(res.size());
		int idx = 0;
		for (auto& i : res)
		{
			out[idx++] = i;
		}
		return out;
		});
}
QFuture<Group::Data> Update::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args;
	if(name.has_value())
		args.emplace("name",*std::move(name));
	if (tag.has_value())
		args.emplace("tag", *std::move(tag));
	args.emplace("roomID",roomID);
	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		return Group::Data(res);
		});
}
QFuture<void> AddUser::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName,{ {"roomID",roomID},{"userID",userID}})
		.then([](json&& res) {
		});
}

QFuture<void> Delete::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, { {"roomID",roomID} })
		.then([](json&& res) {
			});
}
QFuture<std::vector<Message::Data>> GetHistory::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName,{ { "roomID",roomID } }).then([](json&& res) {
		std::vector<Message::Data> out(res.size()); 
		int idx = 0;
		for (auto& i : res)
		{
			out[idx++] = i;
		}
		return out;
		});
}
QFuture<void> MarkRead::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName,{ { "roomID",roomID },{"count",count}})
		.then([](json&& res) {
		});
}
