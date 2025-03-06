#include "groupmodel.h"
Q_LOGGING_CATEGORY(LC_ROOM_MODEL, "GroupModel");
using namespace Group;
static const QHash<int, QByteArray> roles = {
	{Model::IDRole,"id"},
	{Model::NameRole,"name"},
	{Model::HashRole,"hash"},
	{Model::TagRole,"tag"},
	{Model::MessageCountRole,"messageCount"},
};
Model::Model(QObject* parent)
	:IdentifyingModel(roles, parent)
{

}
QVariant Model::read(const Group::ExtendedData& data, int row, int role) const
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
	case MessageCountRole:
		return data.messageCount;
	default:
		break;
	}
}
bool Model::edit(Group::ExtendedData& data, const QVariant& value, int row, int role)
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
		if (value.canConvert<QString>()) {
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
	case MessageCountRole:
		if (value.canConvert<int>())
		{
			data.messageCount = value.toInt();
			return true;
		}break;
		return false;
	}
}
QModelIndex Model::idToIndex(int id) const
{
	return IdentifyingModel::idToIndex(id);
}