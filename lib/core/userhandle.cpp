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
void Handle::setID(int other)
{
	if (_data.id == other)
		return;
	_data.id = other;
	emit idChanged();
}
void Handle::setTag(const QString& other)
{
	if (_data.tag == other)
		return;
	_data.tag = other;
	emit tagChanged();
}
void Handle::setName(const QString& other)
{
	if (_data.name == other)
		return;
	_data.name = other;
	emit nameChanged();
}
void Handle::setStatus(Status other)
{
	if (_data.status == other)
		return;
	_data.status = other;
	emit statusChanged();
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