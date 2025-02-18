#pragma once
#include "network_include.h"
#include "network.h"
#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(LC_RPC_REPLY);
namespace RPC {
	constexpr static int invalidID = -1;
	enum ReplyStatus 
	{
		Unknown,
		Success,
		Error
	};
	NLOHMANN_JSON_SERIALIZE_ENUM(ReplyStatus, 
	{
		{Unknown, nullptr},
		{Success, "success"},
		{Error, "error"}
	})
	class CC_NETWORK_EXPORT Reply
	{
	public:

		explicit Reply();
		int id;
		int repTo;
		std::optional<std::string> error;
		ReplyStatus status;
		json reply;
	};
	void to_json(json& j, const Reply& p);
	void from_json(const json& j, Reply& p);

	class CC_NETWORK_EXPORT MethodCall
	{
	public:
		explicit MethodCall();
		int id;
		std::string method;
		json args;
	};
	void to_json(json& j, const MethodCall& p);
	void from_json(const json& j, MethodCall& p);

	class CC_NETWORK_EXPORT MessageConstructor
	{
	public:
		static Reply      replyMsg(int replyTo, json reply = nullptr);
		static MethodCall methodCallMsg(std::string method, json args = nullptr);
	private:
		static int generateID();
	};

}