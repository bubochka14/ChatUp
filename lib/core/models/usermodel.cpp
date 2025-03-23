#include "usermodel.h"
using namespace User;
static const QHash<int, QByteArray> roles{
	{ Model::NameRole,"name" },
	{ Model::IDRole,"id" },
	{ Model::StatusRole,"status" }
};
Model::Model(QObject* parent)
	:QAbstractListModel(parent)
{}
//bool Model::edit(User::Handle*& d, const QVariant& val, int row, int role)
//{
//	switch (role)
//	{
//	case IDRole:
//		if (val.canConvert<int>())
//		{
//			d->setID(val.toInt());
//			return true;
//		}
//	case NameRole:
//		if (val.canConvert<QString>())
//		{
//			d->setName(val.toString());
//			return true;
//		}
//	case TagRole:
//		if (val.canConvert<QString>())
//		{
//			d->setTag(val.toString());
//			return true;
//		}
//	case StatusRole:
//		break;
//		return false;
//	}
//}
//QVariant Model::read(const User::Handle*& d, int row, int role) const
//{
//	switch (role)
//	{
//	case IDRole:
//		return d->id();
//	case NameRole:
//		return d->name();
//	case TagRole:
//		return d->tag();
//	case StatusRole:
//		return d->status();
//	}
//	return QVariant();
//}