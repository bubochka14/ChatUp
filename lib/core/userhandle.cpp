#include "userhandle.h"
using namespace User;
Handle::Handle(User::Data&& data, QObject* parent)
	:QObject(parent)
	, _data(std::move(data))
{

}
void Handle::release()
{

}
QString Handle::name() const
{
	return _data.name;
}
QString Handle::tag() const
{
	return _data.tag;
}
int Handle::id() const
{
	return _data.id;
}
User::Status Handle::status() const
{
	return _data.status;
}