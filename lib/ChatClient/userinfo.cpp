#include "userinfo.h"
void UserInfo::extractFromHash(const QVariantHash& other)
{
	if (other.contains("id"))
		setId(other["id"].toInt());
	if (other.contains("status"))
	{
		QMetaEnum&& statusEnum = QMetaEnum::fromType<UserInfo::Status>();
		setStatus(static_cast<UserInfo::Status>(statusEnum.keyToValue(other["status"].toString().toStdString().c_str())));
	}
	if (other.contains("name"))
		setName(other["name"].toString());
}
UserInfo::UserInfo(QObject* parent)
	:QObject(parent)
	,_id(-1)
	,_status(Offline)
{}
int UserInfo::id() const
{
	return _id;
}
UserInfo::Status UserInfo::status() const
{
	return _status;
}
void UserInfo::setStatus(Status other)
{
	if (other == _status)
		return;
	_status = other;
	emit statusChanged();
}
QString UserInfo::name() const { return _name; }
void UserInfo::setId(int other) 
{
	if (other == _id)
		return;
	_id = other;
	emit idChanged();
}
void UserInfo::setName(const QString& other)
{
	if (other == _name)
		return;
	_name = other;
	emit nameChanged();
}
QVariantHash UserInfo::toHash() const
{
	QVariantHash out;
	QMetaEnum&& statusEnum = QMetaEnum::fromType<UserInfo::Status>();
	out["id"] = _id;
	out["name"] = _name;
	out["status"] = statusEnum.valueToKey(_status);
	return out;
}
