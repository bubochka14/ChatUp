#include "identifyingmodel.h"
IdentifyingModelBase::IdentifyingModelBase(QObject* parent)
	:QAbstractListModel(parent)
{
	connect(this, &QAbstractItemModel::rowsInserted, this, &IdentifyingModelBase::rowCountChanged);
	connect(this, &QAbstractItemModel::rowsRemoved, this, &IdentifyingModelBase::rowCountChanged);
}