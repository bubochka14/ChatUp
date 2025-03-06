#include "messagemodel.h"
Q_LOGGING_CATEGORY(LC_MESSAGE_MODEL, "Message.Model")
using namespace Message;
bool Model::addSpecialMessageStatus(int row, Model::MessageStatus other)
{
	if (row >= rowCount() || row < 0)
		return false;
	_specialStatuses.insert(row, other);
	emit dataChanged(index(row), index(row), QList<int>() << StatusRole);
	return true;
}
bool Model::removeSpecialMessageStatus(int row)
{
	if (!_specialStatuses.contains(row))
		return false;
	_specialStatuses.remove(row);
	emit dataChanged(index(row), index(row), QList<int>() << StatusRole);
	return true;
}
int Model::userReadings() const
{
	return _userReadings;
}
int Model::foreignReadings() const
{
	return _foreignReadings;
}
void Model::setForeignReadings(int other)
{
	if (_foreignReadings == other)
		return;
	uint32_t lastCount = _foreignReadings;
	_foreignReadings = other;
	int indexOffset = data(index(0), MessageIndexRole).toInt() - (rowCount() - 1);
	emit foreignReadingsChanged();
	auto topIndex = index(lastCount - indexOffset);
	auto bottomIndex = index(_foreignReadings - indexOffset - 1);
	emit dataChanged(index(0), index(rowCount() - 1));
}
void Model::setUserReadings(int other)
{
	if (_userReadings == other)
		return;
	_userReadings = other;
	emit userReadingsChanged();
}
static const QHash<int, QByteArray> roles = {
	{ Model::UserIdRole,"userID" },
	{ Model::BodyRole,"body" },
	{ Model::TimeRole,"time" },
	{ Model::HashRole,"hash" },
	{ Model::StatusRole,"messageStatus" },
	{ Model::MessageIndexRole,"messageIndex" }
};
Model::Model(/*int currentUserID,*/ QObject* parent)
	:IdentifyingModel(roles, parent)
	//, _currentUserID(currentUserID)
	,_userReadings(0)
	,_foreignReadings(0)
{
}
QVariant Model::read(const Message::Data& data, int row, int role) const
{
	switch (role)
	{
	case IdentifyingModel<Message::Data>::IDRole():
		return data.id;
	case UserIdRole:
		return data.userID;
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
		//if (data.userID == _currentUserID)
			return data.messageIndex > _foreignReadings ? Sent : Read;
		//else
		//	return data.messageIndex > userReadMessagesCount() ? Sent : Read;
	}
	default:
		return QVariant();
	}
}
bool Model::edit(Message::Data& data, const QVariant& value, int row, int role)
{
	switch (role)
	{
	case IdentifyingModel<Message::Data>::IDRole():
		if (value.canConvert<int>())
		{
			data.id = value.toInt();
			return true;
		}
	case UserIdRole:
		if (value.canConvert<int>()) {
			data.userID = value.toInt();
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
			data.fromHash(value.value<QVariantHash>());
			return true;
		}
	};
	return false;
}