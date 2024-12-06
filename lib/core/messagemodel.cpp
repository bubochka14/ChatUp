#include "messagemodel.h"
Q_LOGGING_CATEGORY(LC_MESSAGE_MODEL,"MessageModel")
MessageData::MessageData()
	:id(-1)
	,userId(-1)
{

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
QVariantHash MessageData::toHash() const
{
	QVariantHash out;
	out["id"] = id;
	out["time"] = time;
	out["body"] = body;
	out["userID"] = userId;
	out["messageIndex"] = messageIndex;
	return out;
}
void MessageData::extractFromHash(const QVariantHash& other)
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
static const QHash<int, QByteArray> roles = {
	{ MessageModel::UserIdRole,"userID" },
	{ MessageModel::BodyRole,"body" },
	{ MessageModel::TimeRole,"time" },
	{ MessageModel::HashRole,"hash" },
	{ MessageModel::StatusRole,"messageStatus" },
	{ MessageModel::MessageIndexRole,"messageIndex" }
};
MessageModel::MessageModel(int currentUserID,QObject* parent )
	:IdentifyingModel(roles,parent)
	,_currentUserID(currentUserID)
	,_userReadMessagesCount(0)
	,_foreignReadMessagesCount(0)
{
}
QVariant MessageModel::read(const MessageData& data,int row, int role) const
{
	switch (role)
	{
	case IdentifyingModel<MessageData>::IDRole():
		return data.id;
	case UserIdRole:
		return data.userId;
	case BodyRole:
		return data.body;
	case TimeRole:
		return data.time;
	case MessageIndexRole:
		return data.messageIndex;
	case HashRole:
		return data.toHash();
	case StatusRole:
	{
		if (_specialStatuses.contains(row))
			return _specialStatuses[row];
		if (data.userId == _currentUserID)
			return data.messageIndex > foreignReadMessagesCount() ? Sent : Read;
		else
			return data.messageIndex > userReadMessagesCount() ? Sent : Read;
	}
	default:
		return QVariant();
	}
}
bool MessageModel::edit(MessageData& data,const QVariant& value, int row, int role)
{
	switch (role)	
	{
	case IdentifyingModel<MessageData>::IDRole():
		if (value.canConvert<int>())
		{
			data.id = value.toInt();
			return true;
		}
	case UserIdRole:
		if (value.canConvert<int>()) {
			data.userId = value.toInt();
			return true;
		}
	case BodyRole:
		if (value.canConvert<QByteArray>()) {
			data.body = value.value<QByteArray>();
			return true;
		}
	case TimeRole:
		if (value.canConvert<QDateTime>()) {
			data.time = value.value<QDateTime>();
			return true;
		}
	case MessageIndexRole:
		if (value.canConvert<uint>()) {
			data.messageIndex = value.toUInt();
			return true;
		}
	case HashRole:
		if (value.canConvert<QVariantHash>()) {
			data.extractFromHash(value.value<QVariantHash>());
			return true;
		}
	};
	return false;
}    