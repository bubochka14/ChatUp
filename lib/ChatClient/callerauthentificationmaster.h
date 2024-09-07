#pragma once
#include "authenticationmaster.h"
#include "chatclient_include.h"

class CHAT_CLIENT_EXPORT CallerAuthentificationMaster : public AuthenticationMaster
{
	Q_OBJECT;
public:
	explicit CallerAuthentificationMaster(ServerMethodCaller* caller, QObject* parent = nullptr);
	void loginUser(const QString& login, const QString& password) override;
	void registerUser(const QString& login, const QString& password) override;
private:
	ServerMethodCaller* _caller;
	QFuture<QVariantHash> _authFuture;
};
