#pragma once
#include "data.h"
#include <qloggingcategory.h>
#include "identifyingmodel.h"
#include <qqmlengine.h>
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
namespace Group {
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT);
	;
	class CC_CORE_EXPORT Model : public IdentifyingModel<Group::ExtendedData>
	{
		Q_OBJECT;
		//QML_ELEMENT;
	public:
		enum RoleNames
		{
			IDRole = IdentifyingModel<Group::ExtendedData>::IDRole(),
			NameRole,
			TagRole,
			HashRole,
			HasCallRole

		}; Q_ENUM(RoleNames);
		explicit Model(QObject* parent = nullptr);
		Q_INVOKABLE QModelIndex idToIndex(int id) const;
	protected:
		bool edit(Group::ExtendedData&, const QVariant&, int row, int role) override;
		QVariant read(const Group::ExtendedData&, int row, int role) const override;
	};
}