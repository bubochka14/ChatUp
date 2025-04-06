#include "messagemodel.h"
Q_LOGGING_CATEGORY(LC_MESSAGE_MODEL, "Message.Model")
using namespace Message;
static const QHash<int, QByteArray> roles = {
	{ Model::UserIdRole,"userID" },
	{ Model::BodyRole,"body" },
	{ Model::TimeRole,"time" },
	{ Model::HashRole,"hash" },
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
QModelIndex Model::idToModelIndex(int id)
{
	return IdentifyingModel::idToIndex(id);
}