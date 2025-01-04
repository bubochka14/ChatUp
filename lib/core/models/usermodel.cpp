#include "usermodel.h"
using namespace User;
static const QHash<int, QByteArray> roles{
	{ Model::NameRole,"name" },
	{ Model::IDRole,"id" },
	{ Model::StatusRole,"status" }
};
Model::Model(QObject* parent)
	:IdentifyingModel(roles,parent)
{}
bool Model::edit(Data& d, const QVariant& val, int row, int role)
{
	switch (role)
	{
	case IDRole:
		if (val.canConvert<int>())
		{
			d.id = val.toInt();
			return true;
		}
	case NameRole:
		if (val.canConvert<QString>())
		{
			d.name = val.toString();
			return true;
		}
	case TagRole:
		if (val.canConvert<QString>())
		{
			d.tag = val.toString();
			return true;
		}
	case StatusRole:
		break;
		return false;
	}
}
QVariant Model::read(const Data& d, int row, int role) const
{
	switch (role)
	{
	case IDRole:
		return d.id;
	case NameRole:
		return d.name;
	case TagRole:
		return d.tag;

	case StatusRole:
		return d.status;
	}
	return QVariant();
}