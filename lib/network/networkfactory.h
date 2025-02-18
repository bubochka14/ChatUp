n#pragma once
#include "network_include.h"
#include "memory"
#include "authenticationmaster.h"
#include "networkmanager.h"
#include "controllermanager.h"
#include "connectionhandler.h"
class CC_NETWORK_EXPORT NetworkFactory
{
public:
	virtual std::shared_ptr<AuthenticationMaster> createAuthenticationMaster() = 0;
	virtual std::shared_ptr<ConnectionHandle>	  createConnectionHandle()	   = 0;
	virtual std::shared_ptr<ControllerManager>	  createControllerManager()	   = 0;
	~NetworkFactory() = default;

};
