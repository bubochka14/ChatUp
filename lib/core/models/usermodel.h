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
			StatusRole,
			HandleRole
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
			case HandleRole:
				return QVariant::fromValue(d);
			}
			return QVariant();

		}
		QHash<int, QByteArray> roleNames() const override
		{
			return QHash<int, QByteArray>{
				{ Model::NameRole, "name" },
				{ Model::IDRole,"id" },
				{ Model::StatusRole,"status" },
				{ Model::TagRole,"tag" },
				{ Model::HandleRole,"handle" }
			};
		}
		int rowCount(const QModelIndex& parent = QModelIndex()) const override
		{
			return _data.size();
		}
		void reset(std::vector<User::Handle*> list)
		{
			beginResetModel();
			_data = list;
			for(int i =0; i< _data.size(); i++)
			{
				connect(_data[i], &User::Handle::idChanged, this, [=]()
					{emit dataChanged(index(i), index(i)); });
				connect(_data[i], &User::Handle::nameChanged, this, [=]()
					{emit dataChanged(index(i), index(i)); });
				connect(_data[i], &User::Handle::statusChanged, this, [=]()
					{emit dataChanged(index(i), index(i)); });
			}
			endResetModel();
		}
		void insertHandler(User::Handle* u)
		{

			beginInsertRows(QModelIndex(), _data.size(), _data.size());
			connect(u, &User::Handle::idChanged, this, [=]()
				{emit dataChanged(index(_data.size()), index(_data.size())); });
			connect(u, &User::Handle::nameChanged, this, [=]()
				{emit dataChanged(index(_data.size()), index(_data.size())); });
			connect(u, &User::Handle::statusChanged, this, [=]()
				{emit dataChanged(index(_data.size()), index(_data.size())); });
			_data.push_back(u);

			endInsertRows();
		}
	private:
		std::vector<User::Handle*> _data;
	};
}