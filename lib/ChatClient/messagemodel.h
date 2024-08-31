#pragma once
#include <QAbstractListModel>
#include<QStandardItemModel>
#include <qhash.h>
#include <qqmlengine.h>
#include "qloggingcategory.h"
Q_DECLARE_LOGGING_CATEGORY(LC_MESSAGE_MODEL);
class MessageModel : public QAbstractListModel
{
	Q_OBJECT;
	QML_ELEMENT;
	struct MessageData{ 
		MessageData();
		QVariantHash toHash() const;
		void extractFromHash(const QVariantHash& other);
		int  id;
		int  userId;
		int  roomId;
		QDateTime  time;
		QByteArray body;
	};

public:
	enum RoleNames
	{
		IdRole,
		UserIdRole,
		RoomIdRole,
		BodyRole,
		TimeRole,
		HashRole
	};
	explicit MessageModel(QObject* parent = nullptr);
	bool setData(const QModelIndex& index, const QVariant& value, int role = IdRole) override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	QVariant data(const QModelIndex& index, int role = IdRole) const override;
	QModelIndex idToIndex(int id) const;
private:
	QHash<int, QByteArray> roleNames() const;
	QVector<MessageData>   _messages;
	static const QHash<int, QByteArray> _roleNames;
	QMap<int, int> _idToIndex;
};
Q_DECLARE_OPAQUE_POINTER(MessageModel)