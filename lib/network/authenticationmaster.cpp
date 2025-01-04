#include "authenticationmaster.h"
std::optional<int> AuthenticationMaster::currentUserID() const
{
	return _userID;
}
bool AuthenticationMaster::isAuthenticated() const
{
	return _isAuth;
}
void AuthenticationMaster::setCredentials(Credentials cr)
{
	std::swap(_currentCr, cr);
}
Credentials AuthenticationMaster::currentCredentials()
{
	return _currentCr;
}
AuthenticationMaster::AuthenticationMaster(QObject* parent)
	: QObject(parent) {};
void AuthenticationMaster::setIsAuthenticated(bool other)
{
	if (_isAuth == other)
		return;
	_isAuth = other;
	emit isAuthenticatedChanged();
}
void AuthenticationMaster::setCurrentUserID(int other)
{
	_userID = other;
}
