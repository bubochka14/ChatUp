
#pragma once
#include "idmodel.h"
#include <qhash.h>
#include <qqmlengine.h>
#include "qloggingcategory.h"
#include "core_include.h"
#include <deque>
Q_DECLARE_LOGGING_CATEGORY(LC_MESSAGE_MODEL);
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
class CC_CORE_EXPORT MessageModel : public IdentifyingModel<MessageData>
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
	enum RoleNames
	{
		IdRole = IdentifyingModel<MessageData>::IDRole(),
		UserIdRole ,
		StatusRole,
		BodyRole,
		TimeRole,
		MessageIndexRole,
		HashRole
	}; Q_ENUM(RoleNames)
	explicit MessageModel(int currentUserID,QObject* parent = nullptr);
	bool addSpecialMessageStatus(int row, MessageStatus other);
	bool removeSpecialMessageStatus(int row);
	uint32_t userReadMessagesCount() const;
	uint32_t foreignReadMessagesCount() const;
	void setForeignReadMessagesCount(uint32_t other);
	void setUserReadMessagesCount(uint32_t other);
signals:
	void userReadMessagesCountChanged();
	void foreignReadMessagesCountChanged();
protected:
	bool edit(MessageData&,const QVariant&, int row, int role) override;
	QVariant read(const MessageData&, int row, int role) const override;
private:
	QMap<int, MessageStatus> _specialStatuses;
	int _currentUserID;
	uint32_t _userReadMessagesCount;
	uint32_t _foreignReadMessagesCount;
};
Q_DECLARE_OPAQUE_POINTER(MessageModel)