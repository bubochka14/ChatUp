#include "user.h"
using namespace User;
using namespace User::Api;
using namespace Qt::Literals::StringLiterals;
QFuture<Data> Get::exec(NetworkManager* h)
{

	return h->serverMethod(methodName,{{ "id"_L1,id} })
		.then([](HashList&& res) {
			Data out;
			out.fromHash(std::move(res.takeFirst()));
			return out;
	});
}
QFuture<Data*> Update::exec(NetworkManager* h)
{
	QVariantHash args;
	if (name.has_value())
		args.emplace("name"_L1,*std::move( name));
	if (tag.has_value())
		args.emplace("tag"_L1, *std::move(tag));

	return h->serverMethod(methodName,std::move(args)).then([](HashList&& res) {
		Data* out = new Data;
		out->fromHash(std::move(res.takeFirst()));
		return out;
	});
}
QFuture<QList<int>> Find::exec(NetworkManager* h)
{
	QVariantHash args;
	if (name.has_value())
		args.emplace("name"_L1, *std::move(name));
	if (tag.has_value())
		args.emplace("tag"_L1, *std::move(tag));
	if (id.has_value())
		args.emplace("id"_L1, id.value());
	return h->serverMethod(methodName,std::move(args)).then([](HashList&& res) {
		QList<int>	out(res.size());
		for (size_t i = 0; i < out.length(); i++)
		{
			out[i] = res[i].value("id").toInt();
		}
		return out;
	});
}
