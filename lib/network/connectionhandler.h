#pragma once
#include "network_include.h"
#include <functional>
class CC_NETWORK_EXPORT ConnectionHandle
{
public:
	virtual void connect() = 0;
	virtual bool isConnected() = 0;
	virtual void onConnected(std::function<void()>) = 0;
	virtual void onDisconnected(std::function<void()>) = 0;
};