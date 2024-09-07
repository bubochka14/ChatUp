#pragma once
#include <QAbstractListModel>
#include<QStandardItemModel>
#include <qhash.h>
#include <qqmlengine.h>
#include "qloggingcategory.h"
#include "chatclient_include.h"
Q_DECLARE_LOGGING_CATEGORY(LC_MESSAGE_MODEL);
class CHAT_CLIENT_EXPORT MessageModel : public QAbstractListModel
{
	Q_OBJECT;
	QML_ELEMENT;
public:
	struct MessageData{ 
		MessageData();
		QVariantHash toHash() const;
		void extractFromHash(const QVariantHash& other);
		int  id;
		int  userId;
		int  roomId;
		QDateTime  time;
		QByteArray body;
		static int checkId(const QVariantHash& data,bool& success)
		{
			if (data.contains("id"))
			{
				success = true;
				return data["id"].toInt();
			}
			success = false;
			return 0;
		}
		static int checkRoomId(const QVariantHash& data, bool& success)
		{
			if (data.contains("roomId"))
			{
				success = true;
				return data["roomId"].toInt();
			}
			success = false;
			return 0;
		}
	};
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
	bool updateFromHash(const QVariantHash& hash);
	QVariant data(const QModelIndex& index, int role = IdRole) const override;
	QModelIndex idToIndex(int id) const;
private:
	QHash<int, QByteArray> roleNames() const;
	QVector<MessageData>   _messages;
	static const QHash<int, QByteArray> _roleNames;
	QMap<int, int> _idToIndex;
};
Q_DECLARE_OPAQUE_POINTER(MessageModel)