#include "rpc.h"
using namespace RPC;
Q_LOGGING_CATEGORY(LC_RPC_REPLY, "RPC Reply");
Reply::Reply()
	:repTo(invalidID)
	,id(invalidID)
	,status(Unknown)
{}
MethodCall::MethodCall()
	:id(invalidID)
{}
void RPC::to_json(json& j, const Reply& p)
{
	j = {
		{"type","response"},
		{"messageID",p.id},
		{"data",
		{
			{"responseTo",p.repTo},
			{"status", p.status},
			{"return",p.reply}
		} }
	};
	if (p.error.has_value())
		j["errorString"]=p.error.value();

}
void RPC::from_json(const json& j, Reply& p)
{
	try {
		j.at("messageID").get_to(p.id);
		auto data = j.at("data");
		p.status = data["status"].template get<ReplyStatus>();
		data.at("responseTo").get_to(p.repTo);
		data.value("return",json()).get_to(p.reply);
		if(data.contains("errorString"))
			p.error = data.at("errorString");
	}
	catch(std::exception ex)
	{ 
		qCWarning(LC_RPC_REPLY) << "Reply parsing error: " << ex.what();

	}
}
int MessageConstructor::generateID()
{
	static int i = 1;
	return ++i;
}
Reply MessageConstructor::replyMsg(int replyTo, json reply)
{

	Reply out;
	out.id = generateID();
	out.repTo = replyTo;
	out.reply = std::move(reply);
	return out;
}
MethodCall MessageConstructor::methodCallMsg(std::string method, json args)
{
	MethodCall out;
	out.id = generateID();
	out.method = std::move(method);
	out.args = std::move(args);
	return out;
}
void RPC::to_json(json& j, const MethodCall& p) {
	j = { 
		{"type","methodCall"},
		{"messageID",p.id},
		{"data",{
			{"method", p.method},
			{"args", p.args}
		}}
	};
}
void RPC::from_json(const json& j, MethodCall& p) {
	try {
		p.id = j.at("messageID");
		auto data = j.at("data");
		data.at("method").get_to(p.method);
		data.value("args",json()).get_to(p.args);
	}catch(...)
	{ }
}