#pragma once 
#include <QObject>
#include "messageconstructor.h"
#include "wsclient.h"
#include "servermethodcaller.h"
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(LC_AuthMaster)
class AuthenticationMaster : public QObject
{
	Q_OBJECT;
	Q_PROPERTY(bool isAuthenticated READ isAuthenticated NOTIFY authenticatedChanged);
	Q_PROPERTY(QString userToken READ userToken NOTIFY userTokenChanged);
	Q_PROPERTY(UserInfo userInfo READ userInfo NOTIFY userInfoChanged);
	QString   _token;
	bool      _authenticated;
	UserInfo  _userInfo;
	QString   _lastError;
public:
	explicit AuthenticationMaster(QObject* parent = nullptr);
	UserInfo userInfo() const; //current user 
	bool     isAuthenticated() const;
	QString  userToken() const;
	QString  errorString() const;


public slots:
	//blocking method
	bool loginUser(WSClient*cl, const UserCredentials& cr, int maxAwaitTime);
	//blocking method
	bool registerUser(WSClient* cl,const UserCredentials& cr, int maxAwaitTime);
signals:
	void authenticatedChanged();
	void userTokenChanged();
	void userInfoChanged();
protected slots:
	void setAuthenticated(bool st);
	void setUserToken(const QString& other);
	void setUserInfo(const UserInfo& other);
};
