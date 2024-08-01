#include "authenticationmaster.h"
Q_LOGGING_CATEGORY(LC_AuthMaster, "Authentication")
UserCredentials::UserCredentials(const QString username, const QString& password)
	:password(password)
	,username(username)
{

}
AuthenticationMaster::AuthenticationMaster( QObject* parent)
	:QObject(parent)
	,_authenticated(false)
{

}

bool AuthenticationMaster::isAuthenticated() const
{
	return _authenticated;
}
QString  AuthenticationMaster::userToken() const
{
	return _token;
}
void  AuthenticationMaster::setUserToken(const QString& other)
{
	if (_token != other)
	{
		_token = other;
		emit userTokenChanged();
	}
}
void AuthenticationMaster::setUserInfo(const UserInfo& other)
{
	_userInfo = other;
	emit userInfoChanged();
}
UserInfo AuthenticationMaster::userInfo() const
{
	return _userInfo;
}
QString  AuthenticationMaster::errorString() const
{
	return _lastError;
}
void AuthenticationMaster::setAuthenticated(bool st)
{
	if (_authenticated != st)
	{
		_authenticated = st;
		emit authenticatedChanged();
	}
}
bool AuthenticationMaster::loginUser(WSClient* cl,const UserCredentials& cr, int time)
{
	ServerMethodCaller caller(cl);
	QString newToken;
	try
	{
		auto call = caller.loginUser(cr)
			.then([&](const QString& t) {
			newToken = t;

			return caller.getCurrentUserInfo(t).result();
				})
			.then([&](UserInfo user) {
					setUserToken(newToken);
					setUserInfo(user);
					setAuthenticated(true);
					return true;
				}
			);
				call.waitForFinished();
				return call.result();
	}catch(const MethodCallFailure& err)
	{
		_lastError = err.errorString();
		return false;
	}catch(...)
	{
		qCCritical(LC_AuthMaster, "Unknow error");
		_lastError = "Unknow error";
		return false;
	}
}
bool AuthenticationMaster::registerUser(WSClient* cl,const UserCredentials& cr, int time)
{
	ServerMethodCaller caller(cl);
	QString newToken;
	try
	{
		auto call = caller.registerUser(cr)
			.then([&](const QString& t) {
			newToken = t;
			return caller.getCurrentUserInfo(t).result();
				})
			.then([&](UserInfo user) {
					setUserToken(newToken);
					setUserInfo(user);
					setAuthenticated(true);
					return true;
			});
		call.waitForFinished();
		return call.takeResult();
	}
	catch (const MethodCallFailure& err)
	{
		_lastError = err.errorString();
		return false;
	}
	catch (...)
	{
		qCCritical(LC_AuthMaster, "Unknow error");
		_lastError = "Unknow error";
		return false;
	}
}
