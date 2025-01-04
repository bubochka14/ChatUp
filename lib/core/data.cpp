#include "data.h"

void Group::Data::fromHash(QVariantHash other)
{
		id = other.take("id").toInt();
		name = other.take("name").toString();
		tag = other.take("tag").toString();
		messageCount = other.take("messageCount").toInt();
}

QVariantHash Group::Data::toHash() const
{
	QVariantHash out;
	out["id"] = id;
	out["messageCount"] = messageCount;
	if (!name.isEmpty())
		out["name"] = name;
	if (!tag.isEmpty())
		out["tag"] = tag;
	return out;
}

void User::Data::fromHash(QVariantHash other)
{
	if (other.contains("id"))
		id = other.take("id").toInt();
	if (other.contains("status"))
	{
		QMetaEnum&& statusEnum = QMetaEnum::fromType<User::Status>();
		status = static_cast<User::Status>(
			statusEnum.keyToValue(other.take("status").toString().toStdString().c_str()));
	}
	if (other.contains("name"))
		name = other.take("name").toString();
	if (other.contains("tag"))
		tag = other.take("tag").toString();
}
QVariantHash User::Data::toHash() const
{
	QVariantHash out;
	QMetaEnum&& statusEnum = QMetaEnum::fromType<User::Status>();
	out["id"] = id;
	out["name"] = name;
	out["status"] = statusEnum.valueToKey(status);
	out["tag"] = tag;
	return out;
}

QVariantHash Message::Data::toHash() const
{
	QVariantHash out;
	out["id"] = id;
	out["userID"] = userID;
	out["time"] = time;
	out["body"] = body;
	out["messageIndex"] = messageIndex;
	return out;
}
void Message::Data::fromHash(QVariantHash other)
{
	id = other.take("id").toInt();
	userID = other.take("userID").toInt();
	roomID = other.take("roomID").toInt();
	time = other.take("time").toDateTime();
	body = other.take("body").toByteArray();
	messageIndex = other.take("messageIndex").toUInt();
}	
bool Message::Data::isValid() const
{
	return id == defaultID;
}
QVariantHash Participate::Data::toHash() const
{
	return QVariantHash();
}
void Participate::Data::fromHash(QVariantHash hash)
{

}
void Direct::Data::fromHash(QVariantHash other)
{

}
QVariantHash Direct::Data::toHash() const
{
	return QVariantHash();
}