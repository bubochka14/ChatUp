#include "chatroommodel.h"
Q_LOGGING_CATEGORY(LC_ROOM_MODEL, "ChatRoomModel");
RoomData::RoomData(QSharedPointer<MessageHistoryModel> model,const ChatRoom& room )
	:roomInfo(room)
	,history(model)
{

}
MessageHistoryModel* ChatRoomModel::getRoomHistory(int row)
{
	return data(index(row), HistoryModelRole).value<MessageHistoryModel*>();
}
const QHash<int, QByteArray> ChatRoomModel::_roleNames = QHash<int, QByteArray>({
	{NameRole,"roomName" },
	{IDRole,"roomID"},
	{RoomInfoRole, "roomObj"},
	{HistoryModelRole, "messagesModel"}
	}
);
ChatRoomModel::ChatRoomModel(QObject* parent)
	:QAbstractListModel(parent)
{
}
void ChatRoomModel::extractFromRoomList(const RoomList& list)
{

	removeRows(0, rowCount());
	for (auto& i : list)
	{
		addRoom(i, 0);
	}
}
int ChatRoomModel::rowCount(const QModelIndex& parent) const 
{
	Q_UNUSED(parent);
	return _rooms.size();
}
QVariant ChatRoomModel::data(const QModelIndex& index, int role) const 
{
	if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		return QVariant();
	switch (role)
	{
	case IDRole:
		return QVariant::fromValue(_rooms.at(index.row()).roomInfo.roomID);
	case NameRole:
		return _rooms.at(index.row()).roomInfo.name;
	case RoomInfoRole:
		return QVariant::fromValue(_rooms.at(index.row()).roomInfo);
	case HistoryModelRole:
		return QVariant::fromValue(_rooms.at(index.row()).history.get());
	default:
		return QVariant();
	}
	
}
bool ChatRoomModel::insertRows(int row, int count, const QModelIndex& parent) 
{
	Q_UNUSED(parent);
	if (row < 0 || row >rowCount())
		return false;

	beginInsertRows(parent,row, row + count - 1);
	for (size_t n = 0; n < count; n++)
    {
        _rooms.insert(row + n, RoomData{ QSharedPointer<MessageHistoryModel>::create(this)});
		qCDebug(LC_ROOM_MODEL) << "Inserted new row in pos: " << row + n << " row count is: " << rowCount();
	}
	endInsertRows();
	return true;
}
bool ChatRoomModel::removeRows(int row, int count, const QModelIndex& parent ) 
{
	Q_UNUSED(parent);
	if (row<0 || row + count > rowCount())
		return false;
	if (!count)
		return true;
	beginRemoveRows(parent, row, row + count - 1);
	_rooms.remove(row, count);
	qCDebug(LC_ROOM_MODEL) << "Removed "<<   count << " rows from : " << row << " to: " << row + count -1 << "row count is : " << rowCount();

	endRemoveRows();
	return true;
}
QHash<int, QByteArray> ChatRoomModel::roleNames() const
{
	return _roleNames;
}
bool ChatRoomModel::addRoom(const ChatRoom& room, int row)
{
	if (!insertRow(row))
		return false;
	if (setData(index(row), QVariant::fromValue(room),RoomInfoRole))
		return true;
	else
		removeRow(row);
	return false;
}	
bool ChatRoomModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid() || !value.isValid() || index.row() >= rowCount() || index.row() < 0)
	{
		return false;
	}
	switch (role)
	{
	case IDRole:
		_rooms[index.row()].roomInfo.roomID = value.toUInt();
		return true;
	case NameRole:
		_rooms[index.row()].roomInfo.name = value.toString();
		return true;
	case RoomInfoRole:
		if (value.canConvert<ChatRoom>())
		{
			_rooms[index.row()].roomInfo = value.value<ChatRoom>();
			return true;
		}
		else
			return false;
	case HistoryModelRole:
		if (value.canConvert<MessageHistoryModel*>())
		{
			_rooms[index.row()].history.reset(value.value<MessageHistoryModel*>());
			return true;
		}
		else
			return false;

	default: 
		return false;
	}
}
