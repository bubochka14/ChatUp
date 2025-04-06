#include "user.h"
Q_LOGGING_CATEGORY(LC_USER_API, "UserApi");

using namespace User;
using namespace User::Api;
using namespace Qt::Literals::StringLiterals;
void Update::handle(std::shared_ptr<NetworkCoordinator> coord, std::function<void(UserUpdate)> cb)
{
	coord->addClientHandler(methodName, [h = std::move(cb)](json&& res) {
		UserUpdate out;
		try {
			out.id = res["id"];
			if (res.contains("tag"))
				out.tag.emplace(std::move(res["tag"]));
			if (res.contains("name"))
				out.name.emplace(std::move(res["name"]));
			if (res.contains("status"))
			
				out.status = res["status"]=="online" ? User::Online : User::Offline;
		}
		catch (nlohmann::json::exception& e)
		{
			qCWarning(LC_USER_API) << "Update call parsing error" << e.what();
		}
		h(std::move(out));

		});

}
QFuture<void> Create::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(std::string(methodName),{ 
			{"login",login },{"password",password} 
		}, NetworkCoordinator::DirectCall).then([](json&& res) {});
}

QFuture<Data> Get::exec(std::shared_ptr<NetworkCoordinator> h)
{
	QFuture<User::Data> future;
	if(id.has_value())
		future = h->serverMethod(methodName, { { "id",id.value()}})
		.then([](json&& res) {
				return Data(res);
			});
	else future = h->serverMethod(methodName, {}).then([](json&& res) {return Data(res); });
	return future;
}
QFuture<Data> Update::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args;
	if (name.has_value())
		args.emplace("name",*std::move( name));
	if (tag.has_value())
		args.emplace("tag", *std::move(tag));

	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		return Data(res);
	});
}
void Find::extractFromQHash(const QVariantHash& h)
{
	if (h.contains("name"))
		name = h["name"].toString().toStdString();
	if (h.contains("tag"))
		tag = h["tag"].toString().toStdString();
	if (h.contains("id"))
		id = h["id"].toString().toStdString();

}
QFuture<std::vector<Data>> Find::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args;
	if (name.has_value())
		args.emplace("name", *std::move(name));
	if (tag.has_value())
		args.emplace("tag", *std::move(tag));
	if (id.has_value())
		args.emplace("id", id.value());
	if (limit.has_value())
		args.emplace("limit",limit.value());
	if (useRegexp)
		args.emplace("regexp", true);
	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		std::vector<Data> out(res.size());
		int idx = 0;
		for (auto& i : res)
		{
			out[idx++] = i;
		}
		return out;
	});
}
