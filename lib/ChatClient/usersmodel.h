#pragma once
#include <QAbstractListModel>
#include<QStandardItemModel>
#include <qhash.h>
#include "userinfo.h"
#include <qloggingcategory.h>
#include "chatclient_include.h"
Q_DECLARE_LOGGING_CATEGORY(LC_USER_MODEL)
class CHAT_CLIENT_EXPORT UsersModel : public QAbstractListModel
{
	Q_OBJECT;
	QVector<UserInfo*> _users;
	QMap<int, int>     _idToRow;
	static const QHash<int, QByteArray> _roleNames;
public:
	enum RoleNames
	{
		IdRole = Qt::UserRole,
		NameRole,
		TagRole,
		StatusRole
	};
	explicit UsersModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	//bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	//bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	QVariant data(const QModelIndex& index, int role = IdRole) const override;
	//bool setData(const QModelIndex& index, const QVariant& value, int role) override;
	QModelIndex idToIndex(int id) const;
public slots:
	bool pushUser(UserInfo* user);
	bool popUser(int id);
private:
	QHash<int, QByteArray> roleNames() const;

};
