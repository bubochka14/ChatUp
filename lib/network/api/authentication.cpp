//#include "authentication.h"
//using namespace Api;
//using namespace Qt::Literals::StringLiterals;
//void Login::fromCredentials(Credentials other)
//{
//	login = std::move(other.login);
//	password = std::move(other.password);
//}
//
//QFuture<User::Data> Login::exec(std::shared_ptr<NetworkCoordinator> h)
//{
//	return h->serverMethod(methodName, { 
//		{"login",login },{"password",password} }, NetworkCoordinator::DirectCall)
//		.then([](json&& res) {
//		return User::Data(res);
//			});
//}