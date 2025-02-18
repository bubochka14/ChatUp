#include "message.h"
using namespace Message::Api;
using namespace Message;
using namespace Qt::Literals::StringLiterals;
void Create::handle(std::shared_ptr<NetworkCoordinator> serv, std::function<void(Data&&)> h)
{
	static constexpr char methodName[] = "postMessage";
	serv->addClientHandler(methodName,[handler = std::move(h)](json&& res) {
		handler(Message::Data(res));
		});

}
QFuture<std::vector<Data>> Find::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return QtFuture::makeReadyValueFuture(std::vector<Data>());
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
QFuture<std::vector<Data>> GetByIndex::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, { 
			{"from",startIndex },
			{"to",endIndex},
			{"roomID",roomID} 
		}).then([](json&& res) {
			std::vector<Data> out(res.size());
			int idx = 0;
			for (auto& i : res)
			{
				out[idx++] = i;
			}
			return out;
		});
}

QFuture<Data> Create::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName, { { "body",std::move(body) },
		{"roomID",roomID} }).then([](json&& res) {
		return Data(res);
	});
}
QFuture<Data> Update::exec(std::shared_ptr<NetworkCoordinator> h)
{
	json args = { {"body",std::move(body)}};
	return h->serverMethod(methodName,std::move(args)).then([](json&& res) {
		return Data(res);
		});

}

QFuture<void> Delete::exec(std::shared_ptr<NetworkCoordinator> h)
{
	return h->serverMethod(methodName,{ {"id"_L1,messageID} }).then([](json&&) {});
}