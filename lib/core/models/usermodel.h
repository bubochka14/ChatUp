#pragma once
#include "data.h"
#include <qloggingcategory.h>
#include "identifyingmodel.h"
#include <qqmlengine.h>
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
namespace User {
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT);

	class CC_CORE_EXPORT Model : public IdentifyingModel<Data>
	{
	public:
		enum RoleNames
		{
			IDRole = IdentifyingModel<Data>::IDRole(),
			NameRole,
			TagRole,
			StatusRole
		};
		explicit Model(QObject* parent = nullptr);
		bool edit(Data&, const QVariant&, int row, int role) override;
		QVariant read(const Data&, int row, int role) const override;
	};
}