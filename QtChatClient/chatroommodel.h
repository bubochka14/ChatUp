#pragma once
#include <QAbstractListModel>
#include "messagehistorymodel.h"
#include "chatroomstructs.h"
#include <QLoggingCategory>
#include "messagehistorymodel.h"
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
struct RoomData
{
public:
	explicit RoomData(const ChatRoom& room = ChatRoom(), QSharedPointer<MessageHistoryModel> model = QSharedPointer<MessageHistoryModel>::create());
	ChatRoom roomInfo;
	QSharedPointer<MessageHistoryModel> history;
};
class ChatRoomModel : public QAbstractListModel
{
	Q_OBJECT;
	QList<RoomData> _rooms;
	static const QHash<int, QByteArray> _roleNames;
public:
	enum RoleNames
	{
		NameRole = Qt::UserRole,
		IDRole,
		RoomInfoRole,
		HistoryModelRole
	};
	explicit ChatRoomModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = NameRole) const override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	void extractFromRoomList(const RoomList& list);
	bool addRoom(const ChatRoom& room, int row);
	bool setData(const QModelIndex& index, const QVariant& value, int role = NameRole) override;
private:
	QHash<int, QByteArray> roleNames() const;
};