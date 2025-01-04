#include "authentication.h"
using namespace Api;
using namespace Qt::Literals::StringLiterals;
QFuture<User::Data> Register::exec(NetworkManager* h)
{
	return h->serverMethod(methodName,{ {"login"_L1,login },{"password"_L1,password} },NetworkManager::DirectCall)
		.then([](HashList&& res) {
		User::Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
			});

}
QFuture<User::Data> Login::exec(NetworkManager* h)
{
	return h->serverMethod(methodName, { {"login"_L1,login },{"password"_L1,password} }, NetworkManager::DirectCall)
		.then([](HashList&& res) {
		User::Data out;
		out.fromHash(std::move(res.takeFirst()));
		return out;
			});
}