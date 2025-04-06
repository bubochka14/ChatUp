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
	class CC_CORE_EXPORT Model : public IdentifyingModel<Message::Data,std::deque,MutexLock>
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(MessageModel);
		QML_UNCREATABLE("");
	public:
		enum MessageStatus { Loading, Sent, Read, Error };
		Q_ENUM(MessageStatus);
		enum RoleNames
		{
			IdRole = IdentifyingModel<Data>::IDRole(),
			UserIdRole,
			BodyRole,
			TimeRole,
			MessageIndexRole,
			HashRole
		}; Q_ENUM(RoleNames);
		explicit Model(QObject* parent = nullptr);
	protected:
		bool edit(Message::Data&, const QVariant&, int row, int role) override;
		QVariant read(const Message::Data&, int row, int role) const override;
		Q_INVOKABLE QModelIndex idToModelIndex(int id);
	private:
		QMap<int, MessageStatus> _specialStatuses;
		int _currentUserID;
		int _userReadings;
		int _foreignReadings;
	};
}