#include "messagemodel.h"
Q_LOGGING_CATEGORY(LC_MESSAGE_MODEL,"MessageModel")
MessageModel::MessageData::MessageData()
	:id(-1)
	,userId(-1)
{

}
const QHash<int, QByteArray> MessageModel::_roleNames = QHash<int, QByteArray>({
	{IdRole,"id" },
	{UserIdRole,"userID"},
	{BodyRole,"body"},
	{TimeRole,"time"},
	{HashRole,"hash"},
	{StatusRole,"messageStatus"},
	{MessageIndexRole,"messageIndex"}
	}
);
QModelIndex MessageModel::idToIndex(int id) const
{
	if (!_idToRow.contains(id))
	{
		qCWarning(LC_MESSAGE_MODEL) << "Specified message id " << id << " not found";
		return QModelIndex();
	}
	return index(_idToRow[id]);
}
bool MessageModel::addSpecialMessageStatus(int row, MessageModel::MessageStatus other)
{
	if (row >= rowCount() || row < 0)
		return false;	
	_specialStatuses.insert(row, other);
	emit dataChanged(index(row), index(row),QList<int>() << StatusRole);
	return true;
}
bool MessageModel::removeSpecialMessageStatus(int row)
{
	if (!_specialStatuses.contains(row))
		return false;
	_specialStatuses.remove(row);
	emit dataChanged(index(row), index(row), QList<int>() << StatusRole);
	return true;
}
uint32_t MessageModel::userReadMessagesCount() const
{
	return _userReadMessagesCount;
}
uint32_t MessageModel::foreignReadMessagesCount() const
{
	return _foreignReadMessagesCount;
}
void MessageModel::setForeignReadMessagesCount(uint32_t other)
{
	if (_foreignReadMessagesCount == other)
		return;
	uint32_t lastCount = _foreignReadMessagesCount;
	_foreignReadMessagesCount = other;
	int indexOffset = data(index(0), MessageIndexRole).toInt() - (rowCount() - 1);
	emit foreignReadMessagesCountChanged();
	auto topIndex = index(lastCount - indexOffset);
	auto bottomIndex = index(_foreignReadMessagesCount - indexOffset - 1);
	emit dataChanged(index(0), index(rowCount() - 1));
}
void MessageModel::setUserReadMessagesCount(uint32_t other)
{
	if (_userReadMessagesCount == other)
		return;
	_userReadMessagesCount = other;
	emit userReadMessagesCountChanged();
}
QVariantHash MessageModel::MessageData::toHash() const
{
	QVariantHash out;
	out["id"] = id;
	out["time"] = time;
	out["body"] = body;
	out["userID"] = userId;
	out["messageIndex"] = messageIndex;
	return out;
}
void MessageModel::MessageData::extractFromHash(const QVariantHash& other)
{
	if (other.contains("id"))
		id = other["id"].toInt();
	if (other.contains("userID"))
		userId = other["userID"].toInt();
	if (other.contains("time"))
		time = other["time"].toDateTime();
	if (other.contains("body"))
		body = other["body"].toByteArray();	
	if (other.contains("messageIndex"))
		messageIndex = other["messageIndex"].toUInt();
}
MessageModel::MessageModel(int currentUserID,QObject* parent )
	:QAbstractListModel(parent)
	,_currentUserID(currentUserID)
	,_userReadMessagesCount(0)
	,_foreignReadMessagesCount(0)
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
	case BodyRole:
		return _messages.at(index.row()).body;
	case TimeRole:
		return _messages.at(index.row()).time;
	case MessageIndexRole:
		return _messages.at(index.row()).messageIndex;
	case HashRole:
		return _messages.at(index.row()).toHash();
	case StatusRole:
	{
		if (_specialStatuses.contains(index.row()))
			return _specialStatuses[index.row()];
		auto it = _messages.cbegin() + index.row();
		if ((*it).userId == _currentUserID)
			return (*it).messageIndex > foreignReadMessagesCount() ? Sent : Read;
		else
			return (*it).messageIndex > userReadMessagesCount() ? Sent : Read;
	}
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
	if (row + count < _messages.size())
	{
		for (auto i = _messages.begin() + row + 1; i < _messages.end(); ++i)
		{
			_idToRow[i->id] += count;
		}
	}
	_messages.insert(_messages.begin() + row, count, std::move(MessageData()));
	endInsertRows();
	return true;
}
bool MessageModel::removeRows(int row, int count, const QModelIndex& parent)
{
	Q_UNUSED(parent);
	if (row<0 || row + count > rowCount() || count <= 0)
		return false;
	beginRemoveRows(parent, row, row + count - 1);
	_idToRow.remove(_messages[row].id);
	_messages.erase(_messages.begin() + row, _messages.begin() + row+ count);
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
			_idToRow[value.toInt()] = index.row();
			_messages[index.row()].id = value.value<int>();
			emit dataChanged(index, index, QList<int>() << IdRole);
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
	case MessageIndexRole:
		if (value.canConvert<uint>()) {
			_messages[index.row()].messageIndex = value.toUInt();
			emit dataChanged(index, index, QList<int>() << TimeRole);
			return true;
		}
	case HashRole:
		if (value.canConvert<QVariantHash>()) {
			_messages[index.row()].extractFromHash(value.value<QVariantHash>());
			emit dataChanged(index, index);
			return true;
		}
	};
	return false;
}    