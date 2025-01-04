#pragma once 
#include <QObject>
#include "messageconstructor.h"
#include "wsclient.h"
#include <QLoggingCategory>
#include "data.h"
#include "network_include.h"
#include <QFuture>
Q_DECLARE_LOGGING_CATEGORY(LC_AuthMaster)
struct CC_NETWORK_EXPORT Credentials
{
	QString password;
	QString login;
};
class CC_NETWORK_EXPORT AuthenticationMaster : public QObject
{
	Q_OBJECT;
	Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY isAuthenticatedChanged)
public:
	void setCredentials(Credentials cr);
	Credentials currentCredentials();
	virtual QFuture<void> loginUser()=0;
	virtual QFuture<void> registerUser() = 0;
	std::optional<int> currentUserID() const;
	bool isAuthenticated() const;
signals:
	void isAuthenticatedChanged();
	void errorReceived(const QString&);
protected:
	explicit AuthenticationMaster(QObject* parent = nullptr);
	void setIsAuthenticated(bool other);
	void setCurrentUserID(int other);
private:
	std::optional<int> _userID;
	bool _isAuth;
	Credentials _currentCr;
};
