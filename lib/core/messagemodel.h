
#pragma once
#include <QAbstractListModel>
#include<QStandardItemModel>
#include <qhash.h>
#include <qqmlengine.h>
#include "qloggingcategory.h"
#include "core_include.h"
#include <deque>
Q_DECLARE_LOGGING_CATEGORY(LC_MESSAGE_MODEL);
class CC_CORE_EXPORT MessageModel : public QAbstractListModel
{
	Q_OBJECT;
	QML_ELEMENT;
	QML_UNCREATABLE("");
	Q_PROPERTY(uint32_t userReadMessagesCount 
		READ userReadMessagesCount 
		WRITE setUserReadMessagesCount 
		NOTIFY userReadMessagesCountChanged
	)
	Q_PROPERTY(uint32_t foreignReadMessagesCount
		READ foreignReadMessagesCount 
		WRITE setForeignReadMessagesCount
		NOTIFY foreignReadMessagesCountChanged
	)
public:
	enum MessageStatus{Loading,Sent,Read,Error};
	Q_ENUM(MessageStatus);
	struct MessageData {
		MessageData();
		QVariantHash toHash() const;
		void extractFromHash(const QVariantHash& other);
		int  id;
		int  userId;
		QDateTime  time;
		QByteArray body;
		uint32_t messageIndex;
	};
	enum RoleNames
	{
		IdRole,
		UserIdRole,
		StatusRole,
		BodyRole,
		TimeRole,
		MessageIndexRole,
		HashRole
	}; Q_ENUM(RoleNames)
	explicit MessageModel(int currentUserID,QObject* parent = nullptr);
	bool setData(const QModelIndex& index, const QVariant& value, int role = IdRole) override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool addSpecialMessageStatus(int row, MessageStatus other);
	bool removeSpecialMessageStatus(int row);
	
	QVariant data(const QModelIndex& index, int role = IdRole) const override;
	QModelIndex idToIndex(int id) const;
	uint32_t userReadMessagesCount() const;
	uint32_t foreignReadMessagesCount() const;
	void setForeignReadMessagesCount(uint32_t other);
	void setUserReadMessagesCount(uint32_t other);
signals:
	void userReadMessagesCountChanged();
	void foreignReadMessagesCountChanged();
private:
	QHash<int, QByteArray> roleNames() const;
	std::deque<MessageData>   _messages;
	static const QHash<int, QByteArray> _roleNames;
	QMap<int, int> _idToRow;
	QMap<int, MessageStatus> _specialStatuses;
	int _currentUserID;
	uint32_t _userReadMessagesCount;
	uint32_t _foreignReadMessagesCount;
};
Q_DECLARE_OPAQUE_POINTER(MessageModel)