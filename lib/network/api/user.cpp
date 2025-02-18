#include "user.h"
using namespace User;
using namespace User::Api;
using namespace Qt::Literals::StringLiterals;
QFuture<void> Create::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(std::string(methodName),
		{ {"login",login },{"password",password} }, NetworkCoordinator::DirectCall)
		.then([](json&& res) {});

}
QFuture<Data> Get::exec(std::shared_ptr<NetworkCoordinator> h)
{
	QFuture<User::Data> future;
	if(id.has_value())
		future = h->serverMethod(methodName, { { "id",id.value()}})
		.then([](json&& res) 
			{
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
QFuture<std::vector<int>> Find::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args;
	if (name.has_value())
		args.emplace("name", *std::move(name));
	if (tag.has_value())
		args.emplace("tag", *std::move(tag));
	if (id.has_value())
		args.emplace("id", id.value());
	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		std::vector<int> out(res.size());
		int idx = 0;
		for (auto& i : res)
		{
			out[idx++] = i;
		}
		return out;
	});
}
