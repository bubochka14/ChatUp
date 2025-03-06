#pragma once
#include "identifyingmodel.h"
#include <qhash.h>
#include <qqmlengine.h>
#include "qloggingcategory.h"
#include "core_include.h"
#include <deque>
#include "data.h"
namespace Message {
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT);
	class CC_CORE_EXPORT Model : public IdentifyingModel<Message::Data>
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(MessageModel);
		QML_UNCREATABLE("");
		Q_PROPERTY(int userReadings READ userReadings WRITE setUserReadings NOTIFY userReadingsChanged);
		Q_PROPERTY(uint32_t foreignReadings READ foreignReadings WRITE setForeignReadings NOTIFY foreignReadingsChanged)
	public:
		enum MessageStatus { Loading, Sent, Read, Error };
		Q_ENUM(MessageStatus);
		enum RoleNames
		{
			IdRole = IdentifyingModel<Data>::IDRole(),
			UserIdRole,
			StatusRole,
			BodyRole,
			TimeRole,
			MessageIndexRole,
			HashRole
		}; Q_ENUM(RoleNames);
		explicit Model(QObject* parent = nullptr);
		bool addSpecialMessageStatus(int row, MessageStatus other);
		bool removeSpecialMessageStatus(int row);
		int userReadings() const;
		int foreignReadings() const;
		void setForeignReadings(int other);
		void setUserReadings(int other);
	signals:
		void userReadingsChanged();
		void foreignReadingsChanged();
	protected:
		bool edit(Message::Data&, const QVariant&, int row, int role) override;
		QVariant read(const Message::Data&, int row, int role) const override;
	private:
		QMap<int, MessageStatus> _specialStatuses;
		int _currentUserID;
		int _userReadings;
		int _foreignReadings;
	};
}