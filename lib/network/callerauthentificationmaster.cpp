#include "callerauthentificationmaster.h"
CallerAuthenticationMaster::CallerAuthenticationMaster(NetworkManager* handler)
	:AuthenticationMaster(handler)
	,_handler(handler)
{

}
QFuture<void> CallerAuthenticationMaster::loginUser()
{
	Api::Login loginCall;
	loginCall.login = currentCredentials().login;
	loginCall.password = currentCredentials().password;
	return loginCall.exec(_handler).then([this](User::Data&& user) {
		if (!user.id)
			throw QString("invalid user received");

		setCurrentUserID(user.id);
		setIsAuthenticated(true);
		})
		.onFailed([](MethodCallFailure& e) {
		throw e.message;
			});
}
QFuture<void> CallerAuthenticationMaster::registerUser()
{
	Api::Register reg;
	reg.login = currentCredentials().login;
	reg.password = currentCredentials().password;
	return reg.exec(_handler).then([this](User::Data&& user) {
		if (!user.id)
			throw QString("invalid user received");

		setCurrentUserID(user.id);
		setIsAuthenticated(true);
		})
		.onFailed([](MethodCallFailure& e) {
		throw e.message;
			});;
}