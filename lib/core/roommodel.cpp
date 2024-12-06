#include "roommodel.h"
Q_LOGGING_CATEGORY(LC_ROOM_MODEL, "RoomModel");
RoomData::RoomData()
	:id(-1)
	,hasCall(false)
{
}
void RoomData::fromHash(const QVariantHash& other)
{
	if (other.contains("id"))
		id = other["id"].toInt();
	if (other.contains("name"))
		name = other["name"].toString();
	if (other.contains("tag"))
		tag = other["tag"].toString();
	if (other.contains("call"))
		hasCall = other["call"].toBool();
}
QModelIndex RoomModel::idToIndex(int id) const
{
	return IdentifyingModel::idToIndex(id);
}
QVariantHash RoomData::toHash() const
{
	QVariantHash out;
	if(id!=-1)
		out["id"] = id;
	if(!name.isEmpty())
		out["name"] = name;
	if(!tag.isEmpty())
		out["tag"] = tag;
	out["hasCall"] = hasCall;
	return out;
}
static const QHash<int, QByteArray> roles = {
	{RoomModel::IDRole,"id"},
	{RoomModel::NameRole,"name"},
	{RoomModel::HashRole,"hash"},
	{RoomModel::TagRole,"tag"},
	{RoomModel::HasCallRole,"hasCall"},
};
RoomModel::RoomModel(QObject* parent)
	:IdentifyingModel(roles,parent)
{

}
QVariant  RoomModel::read(const RoomData& data, int row, int role) const
{
	switch (role)
	{
	case IDRole:
		return data.id;
	case NameRole:
		return data.name;
	case HashRole:
		return data.toHash();
	case TagRole:
		return data.tag;
	case HasCallRole:
		return data.hasCall;
	default:
		break;
	}
}
bool RoomModel::edit(RoomData& data, const QVariant& value, int row, int role)
{
	switch (role)
	{
	case IDRole:
		if (value.canConvert<int>() && value.toInt() != data.id)
		{
			data.id = value.toInt();
			return true;
		}break;
	case NameRole:
		if(value.canConvert<QString>()){
			data.name = value.toString();
			return true;
		}break;
	case TagRole:
		if (value.canConvert<QString>())
		{
			data.tag = value.toString();
			return true;
		}break;
	case HashRole:
		if (value.canConvert<QVariantHash>())
		{
			data.fromHash(value.toHash());
			return true;
		}break;
	return false;
	}
}
