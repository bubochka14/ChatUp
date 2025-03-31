#include "userhandle.h"
using namespace User;
Handle::Handle(QObject* parent)
	:QObject(parent)
{
	_data.name = "";
	_data.tag = "";
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
Data Handle::data()
{
	return _data;
}
void Handle::extractFromData(User::Data other)
{
	setName(other.name);
	setTag(other.tag);
	setID(other.id);
	setStatus(other.status);
}
void Handle::copy(Handle* other)
{
	extractFromData(other->data());
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