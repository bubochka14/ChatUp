#pragma once 
#include <QObject>
#include "messageconstructor.h"
#include "wsclient.h"
#include "servermethodcaller.h"
#include <QLoggingCategory>
#include "userinfo.h"
#include "chatclient_include.h"

Q_DECLARE_LOGGING_CATEGORY(LC_AuthMaster)
class CHAT_CLIENT_EXPORT AuthenticationMaster : public QObject
{
	Q_OBJECT;
public:
	virtual void loginUser(const QString& login, const QString& password)=0;
	virtual void registerUser(const QString& login, const QString& password) = 0;
signals:
	void authentificated(UserInfo*);
	void errorReceived(const QString&);
protected:
	explicit AuthenticationMaster(QObject* parent = nullptr) : QObject(parent) {};

};
