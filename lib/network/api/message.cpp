#include "message.h"
using namespace Message::Api;
using namespace Message;
using namespace Qt::Literals::StringLiterals;
void Create::handle(NetworkManager* serv, std::function<void(Data&&)> h)
{
	static constexpr char methodName[] = "postMessage";
	serv->addClientHandler([handler = std::move(h)](QVariantHash&& hash) {
		Message::Data out;
		out.fromHash(std::move(hash));
		handler(std::move(out));
		}, methodName);

}
QFuture<QList<Data>> Find::exec(NetworkManager* h)
{
	return QtFuture::makeReadyValueFuture(QList<Data>());
	/*QVariantHash args;
	if (messageIndex.has_value())
		args["messageIndex"] = messageIndex.value();
	if (id.has_value())
		args["id"] = id.value();
	if (userID.has_value())
		args["userID"] = messageIndex.value();
	args["roomID"] = roomID;
	return h->serverMethod(methodName,std::move(args), h).then([](HashList&& list) {
		QList<Data> out(list.size());
		for (size_t i = 0; i < out.size(); i++)
		{
			out.emplace(i,list.takeFirst());
		}
		return out;
		});*/
}
QFuture<QList<Data>> GetByIndex::exec(NetworkManager* h)
{
	return h->serverMethod(methodName, { { "from"_L1,startIndex },
		{"to"_L1,endIndex},{"roomID"_L1,roomID} }).then([](HashList&& res) {
		QList<Data> out(res.size());
		for (size_t i = 0; i < out.length(); i++)
		{
			out[i].fromHash(res.takeFirst());
		}
		return out;
		});
}

QFuture<Data> Create::exec(NetworkManager* h)
{
	return h->serverMethod(methodName, { { "body"_L1,std::move(body) },
		{"roomID",roomID} }).then([](HashList&& res) {
		Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
	});
}
QFuture<Data> Update::exec(NetworkManager* h)
{
	QVariantHash args;
	if (body.has_value())
		args.emplace("body"_L1, *std::move(body));

	return h->serverMethod(methodName,std::move(args)).then([](HashList&& res) {
		Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
		});

}

QFuture<void> Delete::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ {"id"_L1,messageID} }).then([](HashList&&) {});
}