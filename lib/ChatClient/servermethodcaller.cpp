#include "servermethodcaller.h"
void ServerMethodCaller::setTimeout(size_t other)
{
	_timeout = other;
}
size_t ServerMethodCaller::timeout() const
{
	return _timeout;
}
ServerMethodCaller::ServerMethodCaller(QObject* parent)
	:QObject(parent)
	,_timeout(10000)
{}

