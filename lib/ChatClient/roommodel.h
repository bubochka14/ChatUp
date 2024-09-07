#pragma once
#include <QAbstractListModel>
#include <QLoggingCategory>
#include <qqmlengine.h>
#include <qstandarditemmodel.h>
#include <qqmlengine.h>
#include "chatclient_include.h"
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
class CHAT_CLIENT_EXPORT RoomModel : public QAbstractListModel
{
	Q_OBJECT;
	QML_ELEMENT;
public:  
	struct RoomData
	{
		RoomData();
		void fromHash(const QVariantHash& other);
		QVariantHash toHash() const;
		int id;
		QString name;
		QString tag;
		static int checkId(const QVariantHash& data, bool& success)
		{
			if (data.contains("id"))
			{
				success = true;
				return data["id"].toInt();
			}
			success = false;
			return 0;
		}
	};
	QVector<RoomData> _rooms;
	QMap<int, int>    _idToIndex;
	static const QHash<int, QByteArray> _roleNames;
public:
	enum RoleNames
	{
		NameRole = Qt::UserRole,
		IDRole,
		TagRole,
		HashRole
	}; Q_ENUM(RoleNames)
	explicit RoomModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE  QVariant data(const QModelIndex& index, int role = NameRole) const override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	QModelIndex idToIndex(int id);
	bool setData(const QModelIndex& index, const QVariant& value, int role = NameRole) override;
private:
	QHash<int, QByteArray> roleNames() const;
};
