//#include "callerauthentificationmaster.h"
//CallerAuthenticationMaster::CallerAuthenticationMaster(std::shared_ptr<NetworkCoordinator> handler)
//	:_authNetManager(handler)
//{
//
//}
//QFuture<void> CallerAuthenticationMaster::loginUser(Credentials cr)
//{
//	Api::Login loginCall;
//	loginCall.login = std::move(cr.login);
//	loginCall.password = std::move(cr.password);
//	return loginCall.exec(_authNetManager).then([this](User::Data&& user) 
//		{
//			if (!user.id)
//				throw QString("invalid user received");
//
//			setCurrentUserID(user.id);
//			setIsAuthenticated(true);
//			})
//			.onFailed([](MethodCallFailure& e) {
//			throw e.message;
//		});
//}
//QFuture<void> CallerAuthenticationMaster::registerUser(Credentials cr)
//{
//	Api::Register regCall;
//	regCall.login = std::move(cr.login);
//	regCall.password = std::move(cr.password);
//	return regCall.exec(_authNetManager).then([this](User::Data&& user) 
//		{
//			if (!user.id)
//				throw QString("invalid user received");
//			setCurrentUserID(user.id);
//			setIsAuthenticated(true);
//		});
//}