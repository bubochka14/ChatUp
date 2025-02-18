#pragma once
#include "network_include.h"
class CC_NETWORK_EXPORT NetworkInteface
{
public:
	enum Priority
	{
		QueuedCall,
		TopQueuedCall,
		DirectCall
	};
	virtual QFuture<json> serverMethod(
		std::string,
		json args,
		Priority priority = QueuedCall) = 0;
	virtual serverMethod()
};