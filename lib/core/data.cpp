#include "data.h"
using namespace Qt::StringLiterals;
Q_LOGGING_CATEGORY(LC_DATA,"Data");
void Group::ExtendedData::fromHash(QVariantHash other)
{
		id = other.take("id").toInt();
		name = other.take("name").toString();
		tag = other.take("tag").toString();
		messageCount = other.take("messageCount").toInt();
}
void Group::to_json(json& j, const ExtendedData& p)
{
	j = { "data",{
		   {"id", p.id},
		   {"name",p.name.toStdString()},
		   {"tag", p.tag.toStdString()},
		   {"messageCount",p.messageCount}
		   }
	};
}

void Group::from_json(const json& j, ExtendedData& p)
{
	try {
		p.id = j.value("id", Group::invalidID);
		//delete this
		if(j.contains("messageCount"))
		{
			auto mc = j["messageCount"];
			if (!mc.is_null())
				mc.get_to(p.messageCount);
		}
		else
			p.messageCount = 0;
		std::string temp;
		if (j.contains("name"))
		{
			j.at("name").get_to(temp);
			p.name = QString::fromStdString(temp);
		}
		if (j.contains("tag"))
		{
			j.at("tag").get_to(temp);
			p.tag = QString::fromStdString(temp);
		}
	}
	catch (std::exception ex)
	{
		qCWarning(LC_DATA) << "Group parsing error: " << ex.what();
	}
}
QVariantHash Group::ExtendedData::toHash() const
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
void Group::Data::fromHash(QVariantHash other)
{
	id = other.take("id").toInt();
	name = other.take("name").toString();
	tag = other.take("tag").toString();
}
void Group::to_json(json& j, const Data& p)
{
	j = { "data",{
		   {"id", p.id},
		   {"name",p.name.toStdString()},
		   {"tag", p.tag.toStdString()},
		   }
	};
}

void Group::from_json(const json& j, Data& p)
{
	try {
		p.id = j.value("id", Group::invalidID);
		std::string temp;
		if (j.contains("name"))
		{
			j.at("name").get_to(temp);
			p.name = QString::fromStdString(temp);
		}
		if (j.contains("tag"))
		{
			j.at("tag").get_to(temp);
			p.tag = QString::fromStdString(temp);
		}
	}
	catch (std::exception ex)
	{
		qCWarning(LC_DATA) << "Group parsing error: " << ex.what();
	}
}
QVariantHash Group::Data::toHash() const
{
	QVariantHash out;
	out["id"] = id;
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
void User::to_json(json& j, const User::Data& p)
{
	j = { "data",{
	   {"id", p.id},
	   {"name",p.name.toStdString()},
	   {"tag", p.tag.toStdString()},
	   {"status",p.status}
	   }
	};
}
void User::from_json(const json& j, User::Data& p)
{
	try {
		p.id = j.value("id",User::invalidID);
		p.status - j.value("status", User::Offline);
		std::string temp;
		if (j.contains("name"))
		{
			j.at("name").get_to(temp);
			p.name = QString::fromStdString(temp);
		}
		if (j.contains("tag"))
		{
			j.at("tag").get_to(temp);
			p.tag = QString::fromStdString(temp);
		}
	}
	catch (std::exception ex)
	{
		qCWarning(LC_DATA) << "User parsing error: " << ex.what();
	}
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
void Message::to_json(json& j, const Data& p)
{
	j = { "data",{
	   {"id", p.id},
	   {"userID", p.userID},
	   {"roomID", p.roomID},
	   {"time",p.time.toString().toStdString()},
	   {"body", p.body.toStdString()},
	   {"messageIndex",p.messageIndex}
	   }
	};
}
void Message::from_json(const json& j, Data& p)
{
	try {
		p.id = j.value("id", Message::invalidID);
		p.userID = j.value("userID", User::invalidID);
		p.roomID = j.value("roomID", Group::invalidID);
		p.messageIndex = j.value("messageIndex", Message::invalidIndex);
		std::string temp;
		if (j.contains("body"))
		{
			j.at("body").get_to(temp);
			p.body = QByteArray::fromStdString(temp);
		}
		if (j.contains("time"))
		{
			j.at("time").get_to(temp);
			p.time = QDateTime::fromString(QString::fromStdString(temp), Qt::ISODate);
		}
	}
	catch (...)
	{
	}
}
bool Message::Data::isValid() const
{
	return id == invalidID;
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
void Direct::to_json(json& j, const Direct::Data& p)
{

}
void Direct::from_json(const json& j, Direct::Data& p)
{

}
void Participate::to_json(json& j, const Participate::Data& p)
{

}
void Participate::from_json(const json& j, Participate::Data& p)
{
	try {
		p.userID = j.value("participate", User::invalidID);
		p.roomID = j.value("roomID", Group::invalidID);
	}
	catch (...)
	{
		qCWarning(LC_DATA) << "Participate parsing error";
	}
}
QVariantHash Direct::Data::toHash() const
{
	return QVariantHash();
}