#pragma once
#include <QAbstractListModel>
#include <QLoggingCategory>
#include <qqmlengine.h>
#include <qqmlengine.h>
#include "core_include.h"
#include "idmodel.h"
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
struct RoomData
{
	RoomData();
	void fromHash(const QVariantHash& other);
	QVariantHash toHash() const;
	int id;
	QString name;
	QString tag;
	bool hasCall;
	static int checkId(const QVariantHash& data, bool& success)
	{
		if (data.contains("id"))
		{
			success = true;
			return data["id"].toInt();
		}
		success = false;
		return 0;
	}
};
class CC_CORE_EXPORT RoomModel : public IdentifyingModel<RoomData>
{
	Q_OBJECT;
	QML_ELEMENT;
public:  
	enum RoleNames
	{
		IDRole = IdentifyingModel<RoomData>::IDRole(),
		NameRole,
		TagRole,
		HashRole,
		HasCallRole

	}; Q_ENUM(RoleNames)
	explicit RoomModel(QObject* parent = nullptr);
	Q_INVOKABLE QModelIndex idToIndex(int id) const;
protected:
	bool edit(RoomData&, const QVariant&, int row, int role) override;
	QVariant read(const RoomData&, int row, int role) const override;
};
