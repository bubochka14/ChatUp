#include "usersmodel.h"
Q_LOGGING_CATEGORY(LC_USER_MODEL,"UserModel")
const QHash<int, QByteArray> UsersModel::_roleNames = QHash<int, QByteArray>({
	{NameRole,"name" },
	{IdRole,"id"},
	{StatusRole,"status"}
	}
);
UsersModel::UsersModel(QObject* parent)
	:QAbstractListModel(parent)
{
}
int UsersModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return _users.size();
}
bool UsersModel::pushUser(UserInfo* user)
{
	if(_idToRow.contains(user->id()))
	{
		qCCritical(LC_USER_MODEL) << "Cannot push user: user with id: " << user->id() << "already added to model";
		return false;
	}
	beginInsertRows(QModelIndex(), _users.count(), _users.count());
	_users.append(user);
	_idToRow.insert(user->id(), _users.count() - 1);
	connect(user, &UserInfo::idChanged, this, [=]() 
		{emit dataChanged(idToIndex(user->id()), idToIndex(user->id()), QList<int>() << UsersModel::IdRole); });
	connect(user, &UserInfo::nameChanged, this, [=]() 
		{emit dataChanged(idToIndex(user->id()), idToIndex(user->id()), QList<int>() << UsersModel::NameRole); });
	connect(user, &UserInfo::statusChanged, this, [=]() 
		{emit dataChanged(idToIndex(user->id()), idToIndex(user->id()), QList<int>() << UsersModel::StatusRole); });
	endInsertRows();
	return true;
}
bool UsersModel::popUser(int id)
{
	if (!_idToRow.contains(id))
	{
		qCCritical(LC_USER_MODEL) << "Cannot delete user: user with id: " << id << "was not added to model";
		return false;
	}
	beginRemoveRows(QModelIndex(), idToIndex(id).row(), idToIndex(id).row());
	disconnect(_users[idToIndex(id).row()],nullptr, this,nullptr);
	_users.remove(idToIndex(id).row());
	_idToRow.remove(id);
	endRemoveRows();
	return true;

}
QModelIndex UsersModel::idToIndex(int id) const
{
	return QModelIndex();
}

QVariant UsersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		return QVariant();
	switch (role)
	{
	case IdRole:
		return _users.at(index.row())->id();
	case NameRole:
		return _users.at(index.row())->name();
	case StatusRole:
		return _users.at(index.row())->status();
	default:
		return QVariant();
	}

}
//bool UsersModel::insertRows(int row, int count, const QModelIndex& parent)
//{
//	Q_UNUSED(parent);
//	if (row < 0 || row >rowCount())
//		return false;
//
//	beginInsertRows(parent, row, row + count - 1);
//	if (_users.capacity() < _users.length() + count)
//		_users.reserve((_users.length() + count) * 1.7);
//	for (size_t n = 0; n < count; n++)
//	{
//		_users[row + n] = UserInfo();
//	}
//	endInsertRows();
//	return true;
//}
//bool UsersModel::removeRows(int row, int count, const QModelIndex& parent)
//{
//	Q_UNUSED(parent);
//	if (row<0 || row + count > rowCount())
//		return false;
//	if (!count)
//		return true;
//	beginRemoveRows(parent, row, row + count - 1);
//	_users.remove(row, count);
//	endRemoveRows();
//	return true;
//}
QHash<int, QByteArray> UsersModel::roleNames() const
{
	return _roleNames;
}
//bool UsersModel::setData(const QModelIndex& index, const QVariant& value, int role)
//{
//	if (!index.isValid() || !value.isValid() || index.row() >= rowCount() || index.row() < 0)
//	{
//		return false;
//	}
//	switch (role)
//	{
//	case IdRole:
//		if (value.canConvert<int>())
//		{
//			_users[index.row()].id = value.toUInt();
//			emit dataChanged(index, index, QList<int>() << IdRole);
//			return true;
//		}
//	case NameRole:
//		if (value.canConvert<QString>()) {
//			_users[index.row()].name = value.toString();
//			emit dataChanged(index, index,QList<int>()<< NameRole);
//			return true;
//		}
//	}
//	return false;
//}
