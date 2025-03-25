#pragma once
#include "data.h"
#include <qloggingcategory.h>
#include "identifyingmodel.h"
#include <qqmlengine.h>
#include <userhandle.h>
#include <qhash>
#include <qbytearray.h>
Q_DECLARE_LOGGING_CATEGORY(LC_ROOM_MODEL)
namespace User {
	//Q_NAMESPACE_EXPORT(CC_CORE_EXPORT);

	class CC_CORE_EXPORT Model : public QAbstractListModel
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
		QVariant data(const QModelIndex& index, int role) const override final
		{
			if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
				return QVariant();
			auto d = _data[index.row()];
			switch (role)
			{
			case IDRole:
				return d->id();
			case NameRole:
				return d->name();
			case TagRole:
				return d->tag();
			case StatusRole:
				return d->status();
			}
			return QVariant();

		}
		QHash<int, QByteArray> roleNames() const override
		{
			return QHash<int, QByteArray>{
				{ Model::NameRole, "name" },
				{ Model::IDRole,"id" },
				{ Model::StatusRole,"status" }
			};
		}
		int rowCount(const QModelIndex& parent = QModelIndex()) const override
		{
			return _data.size();
		}
		void reset(std::vector<User::Handle*> other)
		{
			beginResetModel();
			_data = other;
			endResetModel();
		}
		void insertHandler(User::Handle* u)
		{
			beginInsertRows(QModelIndex(), _data.size(), _data.size());
			_data.push_back(u);
			endInsertRows();
		}
	private:
		std::vector<User::Handle*> _data;
	};
}