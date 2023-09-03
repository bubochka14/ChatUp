#pragma once
#include <QAbstractListModel>
#include<QStandardItemModel>
#include "chatroomstructs.h"
#include <qhash.h>

class MessageHistoryModel : public QAbstractListModel
{
	Q_OBJECT;
	//Q_PROPERTY(int selectedRoom READ selectedRoom WRITE setSelectedRoom NOTIFY selectedRoomChanged);
	//QHash<RoomHistoryInfo> _data;
	QVector<ChatRoomMessage>   _messages;
	bool _uploaded = false;
	static const QHash<int, QByteArray> _roleNames;

public:
	enum RoleNames
	{
		BodyRole = Qt::UserRole,
		TimeRole,
		UserRole,

	};
	explicit MessageHistoryModel(QObject* parent = nullptr);
	//void setSelectedRoom(int other);
	//int selectedRoom() const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool pushMessage(const ChatRoomMessage& mess);
	bool upload( const MessageList& list);
public slots:
	//void addRooms(const QList<int> id);
	//void removeRooms(const QList<int> id);
private:
	QHash<int, QByteArray> roleNames() const;
signals:
	void needToUpload(ulong roomID);

};
Q_DECLARE_OPAQUE_POINTER(MessageHistoryModel)