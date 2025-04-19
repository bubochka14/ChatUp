#pragma once
#include "data.h"
#include <qloggingcategory.h>
#include "identifyingmodel.h"
#include <qqmlengine.h>
#include <QSortFilterProxyModel>
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
namespace Group {
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT);
	;
class CC_CORE_EXPORT Model : public IdentifyingModel < Group::ExtendedData, std::deque, MutexLock >
	{
		Q_OBJECT;
		QML_NAMED_ELEMENT(GroupModel);
	public:
		enum RoleNames
		{
            IDRole = IdentifyingModel<Message::Data,std::deque,MutexLock>::IDRole(),
			NameRole,
			TagRole,
			HashRole,
			HasCallRole,
			MessageCountRole,
			ForeignReadings,
			LocalReadings,
			LastMessageBody,
			LastMessageSender,
			LastMessageTime

		}; Q_ENUM(RoleNames);
		explicit Model(QObject* parent = nullptr);
		Q_INVOKABLE QModelIndex idToIndex(int id) const;
	protected:
		bool edit(Group::ExtendedData&, const QVariant&, int row, int role) override;
		QVariant read(const Group::ExtendedData&, int row, int role) const override;
	};
	class CC_CORE_EXPORT SortingModel : QSortFilterProxyModel
	{
		Q_OBJECT;
	public:
		explicit SortingModel(QObject* parent = nullptr);
		bool lessThan(const QModelIndex& left,const QModelIndex& right) const override;
	};
}
