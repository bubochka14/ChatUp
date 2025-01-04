#pragma once
#include "core_include.h"
#include <QString>
#include <QVideoSink>
#include "identifyingmodel.h"
#include "data.h"
namespace Participate{
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT)

	class CC_CORE_EXPORT Model : public IdentifyingModel<Participate::Data>
	{
		Q_OBJECT;
	public:
		enum Roles
		{
			UserID = IdentifyingModel<Participate::Data>::IDRole(),
			HasVideo,
			HasAudio
		}; Q_ENUM(Roles);
		explicit Model(QObject* parent = nullptr);
	protected:
		bool edit(Participate::Data&, const QVariant&, int row, int role) override;
		QVariant read(const Participate::Data&, int row, int role) const override;
	};
};