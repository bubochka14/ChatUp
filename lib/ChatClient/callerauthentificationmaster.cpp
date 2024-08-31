#include "callerauthentificationmaster.h"
CallerAuthentificationMaster::CallerAuthentificationMaster(ServerMethodCaller* caller, QObject* parent)
	:AuthenticationMaster(parent)
	,_caller(caller)
{

}
void CallerAuthentificationMaster::loginUser(const QString& login, const QString& password)
{
	UserInfo* newUser = new UserInfo;
	_authFuture = _caller->registerUser(login, password);
	_authFuture.then([=](QVariantHash&& hash)
		{
			newUser->extractFromHash(hash);
			emit authentificated(newUser);
		})
		.onFailed([=](MethodCallFailure* fail)
			{
				QScopedPointer<MethodCallFailure> exScope(fail);
				QScopedPointer<UserInfo> userScope(newUser);
				emit errorReceived(fail->what());
			})
		.onFailed([=]() {
			QScopedPointer<UserInfo>(user);
			emit errorReceived("Authentification fail");
			});
}
void CallerAuthentificationMaster::registerUser(const QString& login, const QString& password)
{
	UserInfo* newUser = new UserInfo;
	_authFuture = _caller->registerUser(login, password);
	_authFuture.then([=](QVariantHash&& hash)
		{
			newUser->extractFromHash(hash);
			emit authentificated(newUser);
		})
		.onFailed([=](MethodCallFailure* fail)
			{
				QScopedPointer<MethodCallFailure> exScope(fail);
				QScopedPointer<UserInfo> userScope(newUser);
				emit errorReceived(fail->what());
			})
		.onFailed([=]() {
			QScopedPointer<UserInfo>(user);
			emit errorReceived("Authentification fail");
			});
}