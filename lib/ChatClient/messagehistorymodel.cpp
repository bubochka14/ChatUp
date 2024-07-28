#include "MessageHistoryModel.h"
const QHash<int, QByteArray> MessageHistoryModel::_roleNames = QHash<int, QByteArray>({
	{BodyRole,"body" },
	{TimeRole,"time"},
	{UserRole, "user"}
	}
);
MessageHistoryModel::MessageHistoryModel(QObject* parent )
	:QAbstractListModel(parent)
{

}
QHash<int, QByteArray> MessageHistoryModel::roleNames() const
{
	return _roleNames;
}

int MessageHistoryModel::rowCount(const QModelIndex& parent) const
{
	return _messages.size();
}
QVariant MessageHistoryModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		return QVariant();
	switch (role)
	{
	case BodyRole:
		return _messages.at(index.row()).body;
	case TimeRole:
		return _messages.at(index.row()).time;
	case UserRole:
		return QVariant::fromValue(_messages.at(index.row()).user);
	default:
		return QVariant();
	}
}
bool MessageHistoryModel::upload(const MessageList& list)
{

	for (auto& i : list)
	{
		pushMessage(i);
	}
	return true;
}

bool  MessageHistoryModel::pushMessage(const ChatRoomMessage& mess)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	_messages.push_back(mess);
	endInsertRows();
	return true;
}