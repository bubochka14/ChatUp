//#pragma once 
//#include <QLoggingCategory>
//#include "data.h"
//#include "network_include.h"
//#include <QFuture>
//Q_DECLARE_LOGGING_CATEGORY(LC_AuthMaster)
//struct CC_NETWORK_EXPORT Credentials
//{
//	std::string password;
//	std::string login;
//};
//class CC_NETWORK_EXPORT AuthenticationMaster
//{
//public:
//	virtual QFuture<void> loginUser(Credentials)=0;
//	virtual QFuture<void> registerUser(Credentials) = 0;
//	virtual QFuture<void> initialize();
//	std::optional<int> currentUserID() const;
//	bool isAuthenticated() const;
//protected:
//	explicit AuthenticationMaster() = default;
//	void setIsAuthenticated(bool other);
//	void setCurrentUserID(int other);
//private:
//	std::optional<int> _userID;
//	bool _isAuth;
//};
