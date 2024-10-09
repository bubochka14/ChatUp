#include "roommodel.h"
Q_LOGGING_CATEGORY(LC_ROOM_MODEL, "RoomModel");
RoomModel::RoomData::RoomData()
	:id(-1)
	,foreignReadCount(0)
	,userReadCount(0)
{
}
void RoomModel::RoomData::fromHash(const QVariantHash& other)
{
	if (other.contains("id"))
		id = other["id"].toInt();
	if (other.contains("name"))
		name = other["name"].toString();
	if (other.contains("tag"))
		tag = other["tag"].toString();
	if (other.contains("userReadCount"))
		userReadCount = other["tag"].toInt();
	if (other.contains("foreignReadCount"))
		foreignReadCount = other["tag"].toInt();
}
QVariantHash RoomModel::RoomData::toHash() const
{
	QVariantHash out;
	if(id!=-1)
		out["id"] = id;
	if(!name.isEmpty())
		out["name"] = name;
	if(!tag.isEmpty())
		out["tag"] = tag;
	return out;
}
const QHash<int, QByteArray> RoomModel::_roleNames = QHash<int, QByteArray>({
	{NameRole,"name" },
	{IDRole,"id"},
	{TagRole, "tag"},
	{HashRole, "hash"},
	{UserReadMessagesCountRole, "userReadCount"},
	{ForeignReadMesssagesCountRole, "foreignReadCount"}
	}
);
RoomModel::RoomModel(QObject* parent)
	:QAbstractListModel(parent)
{
}
int RoomModel::rowCount(const QModelIndex& parent) const 
{
	Q_UNUSED(parent);
	return _rooms.size();
}
QVariant RoomModel::data(const QModelIndex& index, int role) const 
{
	if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		return QVariant();
	switch (role)
	{
	case IDRole:
		return _rooms.at(index.row()).id;
	case NameRole:
		return _rooms.at(index.row()).name;
	case TagRole:
		return _rooms.at(index.row()).tag;
	case HashRole:
		return _rooms.at(index.row()).toHash();
	case ForeignReadMesssagesCountRole:
		return _rooms.at(index.row()).foreignReadCount;
	case UserReadMessagesCountRole:
		return _rooms.at(index.row()).userReadCount;
	default:
		return QVariant();
	}
	
}
bool RoomModel::insertRows(int row, int count, const QModelIndex& parent) 
{
	Q_UNUSED(parent);
	if (row < 0 || row >rowCount() || count<=0) 
		return false;

	beginInsertRows(parent,row, row + count - 1);
	_rooms.insert(row,count, RoomData());
	//update id to index mapping
	if(row+count!=_rooms.count())
	{
		for (auto i = _rooms.begin() + row + 1; i < _rooms.end(); ++i)
		{
			_idToIndex[i->id] += count;
		}
	}
	endInsertRows();
	return true;
}
bool RoomModel::removeRows(int row, int count, const QModelIndex& parent ) 
{
	Q_UNUSED(parent);
	if (row<0 || row + count > rowCount() || count <=0)
		return false;
	beginRemoveRows(parent, row, row + count - 1);
	_idToIndex.remove(_rooms[row].id);
	_rooms.remove(row, count);
	endRemoveRows();
	return true;
}
QHash<int, QByteArray> RoomModel::roleNames() const
{
	return _roleNames;
}
bool RoomModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid() || !value.isValid() || index.row() >= rowCount() || index.row() < 0)
		return false;
 
	switch (role)
	{
	case IDRole:
		if (value.canConvert<int>())
		{
			if (value.toInt() == _rooms[index.row()].id)
				return true;
			int lastId = _rooms[index.row()].id;
			_rooms[index.row()].id = value.toInt();
			_idToIndex[value.toInt()] = index.row();
			_idToIndex.remove(lastId);
			emit dataChanged(index, index, QList<int>() << IDRole);
			return true;
		}break;
	case NameRole:
		if(value.canConvert<QString>()){
			if (value.toString() == _rooms[index.row()].name)
				return true;
			_rooms[index.row()].name = value.toString();
			emit dataChanged(index, index, QList<int>() << NameRole);
			return true;
		}break;
	case TagRole:
		if (value.canConvert<QString>())
		{
			if (value.toString() == _rooms[index.row()].tag)
				return true;
			_rooms[index.row()].tag = value.toString();
			emit dataChanged(index, index, QList<int>() << TagRole);
\
			return true;
		}break;
	case HashRole:
		if (value.canConvert<QVariantHash>())
		{
			int lastId = _rooms[index.row()].id;
			_rooms[index.row()].fromHash(value.toHash());
			if (_rooms[index.row()].id != lastId)
			{
				_idToIndex.remove(lastId);
				_idToIndex.insert(_rooms[index.row()].id, index.row());
			}

			return true;
		}break;
	case ForeignReadMesssagesCountRole:
		if (value.canConvert<uint>()) {
			if (value.toInt() == _rooms[index.row()].foreignReadCount)
				return true;
			_rooms[index.row()].foreignReadCount = value.toUInt();
			emit dataChanged(index, index, QList<int>() << ForeignReadMesssagesCountRole);
			return true;
		}break;
	case UserReadMessagesCountRole:
		if (value.canConvert<uint>()) {
			if (value.toInt() == _rooms[index.row()].userReadCount)
				return true;
			_rooms[index.row()].userReadCount = value.toUInt();
			emit dataChanged(index, index, QList<int>() << UserReadMessagesCountRole);
			return true;
		}break;
	return false;
	}
}
QModelIndex  RoomModel::idToIndex(int id)
{
	if (!_idToIndex.contains(id))
	{
		qCWarning(LC_ROOM_MODEL) << "Specified id " << id << " not found";
		return QModelIndex();
	}
	return index(_idToIndex[id]);
}
