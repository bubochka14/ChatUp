#pragma once
#include "group.h"
using namespace Group::Api;
using namespace Qt::Literals::StringLiterals;

QFuture<Group::Data> Create::exec(NetworkManager* h)
{
	QVariantHash args;
	args.emplace("name"_L1, std::move(name));
	args.emplace("type"_L1,"group"_L1);
	if (tag.has_value())
		args.emplace("tag"_L1, *std::move(tag));
	return h->serverMethod(methodName,std::move(args)).then([](HashList&& res) {
		Group::Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
	});
}
QFuture<QList<Group::Data>> GetAll::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ {"type"_L1,"group"_L1} }).then([](HashList&& res) {
		QList<Group::Data> out(res.size());
		for (size_t i = 0; i < out.size(); i++)
		{
			out[i].fromHash(res.takeFirst());
		}
		return out;
		});
}
QFuture<Group::Data> Update::exec(NetworkManager* h)
{
	QVariantHash args;
	if(name.has_value())
		args.emplace("name"_L1,*std::move(name));
	if (tag.has_value())
		args.emplace("tag"_L1, *std::move(tag));
	args.emplace("roomID"_L1,roomID);
	return h->serverMethod(methodName,std::move(args)).then([](HashList&& res) {
		Group::Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
		});
}
QFuture<void> AddUser::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ {"roomID"_L1,roomID},{"userID"_L1,userID}})
		.then([](HashList&& res) {
		});
}

QFuture<void> Delete::exec(NetworkManager* h)
{
	return h->serverMethod(methodName, { {"roomID"_L1,roomID} })
		.then([](HashList&& res) {
			});
}
QFuture<QList<Message::Data>> GetHistory::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ { "roomID",roomID } }).then([](HashList&& res) {
		QList<Message::Data> out(res.size());
		for (size_t i = 0; i < out.size(); i++)
		{
			out[i].fromHash(res.takeFirst());
		}
		return out;
		});
}
QFuture<void> MarkRead::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ { "roomID",roomID },{"count",count}})
		.then([](HashList&& res) {
		});
}
