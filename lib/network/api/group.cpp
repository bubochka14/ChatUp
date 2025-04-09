#include "group.h"
Q_LOGGING_CATEGORY(LC_GROUP_API,"GroupApi")
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
void AddUser::handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(Desc&&)> h)
{
	net->addClientHandler(methodName, [handler = std::move(h)](json&& res) {
		Desc out;
		out.roomID = res.value("roomID", Group::invalidID);
		out.userID = res.value("userID", User::invalidID);
		handler(std::move(out));
		});
}

QFuture<std::vector<User::Data>> GetUsers::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, { { "roomID",roomID } }).then([](json&& res) {
		std::vector<User::Data> out(res.size());
		int idx = 0;
		for (auto& i : res)
		{
			out[idx++] = i;
		}
		return out;
		});
}
void Update::handle(std::shared_ptr<NetworkCoordinator> net, std::function<void(GroupUpdate&&)> h)
{
	net->addClientHandler(methodName, [handler = std::move(h)](json&& res) {
		if (!res.contains("id"))
		{
			qCWarning(LC_GROUP_API) << "Update call does not contain an room ID";
			return;
		}
		GroupUpdate out;
		try {
			out.id = res["id"];
			if (res.contains("tag"))
				out.tag.emplace(std::move(res["tag"]));
			if (res.contains("name"))
				out.name.emplace(std::move(res["name"]));
			if (res.contains("localReadings"))
				out.localReadings.emplace(std::move(res["localReadings"]));
			if (res.contains("foreignReadings"))
				out.foreignReadings.emplace(std::move(res["foreignReadings"]));
			if (res.contains("messageCount"))
				out.messageCount.emplace(std::move(res["messageCount"]));
			handler((std::move(out)));
		}
		catch (nlohmann::json::exception& e)
		{
			qCWarning(LC_GROUP_API) << "Update call parsing error"<<e.what();
		}
	});
}
void Update::GroupUpdate::extractTo(ExtendedData& data)
{
	if (data.id != id)
	{
		qCWarning(LC_GROUP_API) << "Extract to group data, id mismatch";
		data.id = id;
	}
	if (tag.has_value())
		data.tag = QString::fromStdString(std::move(*tag));
	if (name.has_value())
		data.name = QString::fromStdString(std::move(*name));
	if (foreignReadings.has_value())
		data.foreignReadings = foreignReadings.value();
	if (localReadings.has_value())
		data.localReadings = localReadings.value();
	if (messageCount.has_value())
		data.messageCount = messageCount.value();

}
