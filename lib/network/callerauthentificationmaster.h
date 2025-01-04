#pragma once
#include "authenticationmaster.h"
#include "network_include.h"
#include "serverhandler.h"
#include "api/authentication.h"
class CC_NETWORK_EXPORT CallerAuthenticationMaster : public AuthenticationMaster
{
	Q_OBJECT;
public:
	explicit CallerAuthenticationMaster(NetworkManager* handler);
	QFuture<void> loginUser() override;
	QFuture<void> registerUser() override;
private:
	NetworkManager* _handler;
	std::optional<QFuture<void>> _loginFuture;
	std::optional<QFuture<void>> _registerFuture;
};
