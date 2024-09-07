#include "messagemodel.h"
Q_LOGGING_CATEGORY(LC_MESSAGE_MODEL,"MessageModel")
MessageModel::MessageData::MessageData()
	:id(-1)
	,roomId(-1)
	,userId(-1)
	,status(Loading)
{

}
const QHash<int, QByteArray> MessageModel::_roleNames = QHash<int, QByteArray>({
	{IdRole,"id" },
	{UserIdRole,"userId"},
	{RoomIdRole,"roomId"},
	{BodyRole,"body"},
	{TimeRole,"time"},
	{HashRole,"hash"},
	{StatusRole,"messageStatus"}
	}
);
QModelIndex MessageModel::idToIndex(int id) const
{
	if (!_idToIndex.contains(id))
	{
		qCWarning(LC_MESSAGE_MODEL) << "Specified message id " << id << " not found";
		return QModelIndex();
	}
	return index(_idToIndex[id]);
}

QVariantHash MessageModel::MessageData::toHash() const
{
	QVariantHash out;
	out["id"] = id;
	out["status"] = QMetaEnum::fromType<MessageStatus>().valueToKey(status);
	out["time"] = time;
	out["body"] = body;
	out["userId"] = userId;
	out["roomId"] = roomId;
	return out;
}
void MessageModel::MessageData::extractFromHash(const QVariantHash& other)
{
	if (other.contains("id"))
		id = other["id"].toInt();
	if (other.contains("userId"))
		userId = other["userId"].toInt();
	if (other.contains("roomId"))
		roomId = other["roomId"].toInt();
	if (other.contains("time"))
		time = other["time"].toDateTime();
	if (other.contains("body"))
		body = other["body"].toByteArray();	
	if (other.contains("status"))
		status = MessageStatus(QMetaEnum::fromType<MessageStatus>().
			keyToValue(other["status"].toString().toStdString().c_str()));
}
MessageModel::MessageModel(QObject* parent )
	:QAbstractListModel(parent)
{
}
QHash<int, QByteArray> MessageModel::roleNames() const
{
	return _roleNames;
}

int MessageModel::rowCount(const QModelIndex& parent) const
{
	return _messages.size();
}
QVariant MessageModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		return QVariant();
	switch (role)
	{
	case IdRole:
		return _messages.at(index.row()).id;
	case UserIdRole:
		return _messages.at(index.row()).userId;
	case RoomIdRole:
		return _messages.at(index.row()).roomId;
	case BodyRole:
		return _messages.at(index.row()).body;
	case TimeRole:
		return _messages.at(index.row()).time;
	case HashRole:
		return _messages.at(index.row()).toHash();
	case StatusRole:
		return _messages.at(index.row()).status;
	default:
		return QVariant();
	}
}
bool MessageModel::insertRows(int row, int count, const QModelIndex& parent)
{
	Q_UNUSED(parent);
	if (row < 0 || row >rowCount() || count <= 0)
		return false;
	beginInsertRows(parent, row, row + count - 1);
	_messages.insert(row, count, MessageData());
	endInsertRows();
	return true;
}
bool MessageModel::removeRows(int row, int count, const QModelIndex& parent)
{
	Q_UNUSED(parent);
	if (row<0 || row + count > rowCount() || count <= 0)
		return false;
	beginRemoveRows(parent, row, row + count - 1);
	_idToIndex.remove(_messages[row].id);
	_messages.remove(row, count);
	endRemoveRows();
	return true;
}
bool MessageModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid() || !value.isValid() || index.row() >= rowCount() || index.row() < 0)
	{
		return false;
	}
	switch (role)	
	{
	case IdRole:
		if (value.canConvert<int>())
		{
			_messages[index.row()].id = value.value<int>();
			emit dataChanged(index, index, QList<int>() << IdRole);
			return true;
		}
	case RoomIdRole:
		if (value.canConvert<int>()) {
			_messages[index.row()].roomId = value.value<int>();
			emit dataChanged(index, index, QList<int>() << RoomIdRole);
			return true;
		}
	case UserIdRole:
		if (value.canConvert<int>()) {
			_messages[index.row()].userId = value.value<int>();
			emit dataChanged(index, index, QList<int>() << UserIdRole);
			return true;
		}
	case BodyRole:
		if (value.canConvert<QByteArray>()) {
			_messages[index.row()].body = value.value<QByteArray>();
			emit dataChanged(index, index, QList<int>() << BodyRole);
			return true;
		}
	case TimeRole:
		if (value.canConvert<QDateTime>()) {
			_messages[index.row()].time = value.value<QDateTime>();
			emit dataChanged(index, index, QList<int>() << TimeRole);
			return true;
		}
	case HashRole:
		if (value.canConvert<QVariantHash>()) {
			_messages[index.row()].extractFromHash(value.value<QVariantHash>());
			emit dataChanged(index, index);
			return true;
		}
	case StatusRole:
		if (value.canConvert<MessageStatus>()) {
			_messages[index.row()].status = value.value<MessageStatus>();
			emit dataChanged(index, index, QList<int>() << StatusRole);
			return true;
		}
	};
	return false;
}    