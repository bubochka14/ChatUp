#pragma once
#include <QAbstractItemModel>
#include <unordered_map>
#include <deque>
#include <qloggingcategory.h>
#include "core_include.h"
class CC_CORE_EXPORT IdentifyingModelBase : public QAbstractListModel
{
	Q_OBJECT;
	Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged);

public:
	explicit IdentifyingModelBase(QObject* parent = nullptr);
signals:
	void rowCountChanged();
};
template<class T, template<class>class C = std::deque>
class IdentifyingModel : public IdentifyingModelBase
{
public:
	explicit IdentifyingModel(const QHash<int,QByteArray> & roles,QObject* parent)
		:IdentifyingModelBase(parent)
	{
		_roles.insert(roles);
	}
	QVariant data(const QModelIndex& index, int role) const override final
	{
		if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
			return QVariant();
		return read(_data[index.row()], index.row(), role);

	}
	void reset()
	{
		beginResetModel();
		_data.clear();
		_index.clear();
		endResetModel();
	}
	QVariant data(int id, int role) const 
	{
		if (!_index.contains(id))
			return QVariant();
		return read(_data[id], _index.at(id), role);

	}
	bool setData(const QModelIndex& index, const QVariant& value, int role) override final
	{
		if (!index.isValid() || !value.isValid() || 
			index.row() >= rowCount() || index.row() < 0)
		{
			return false;
		}
		if (role == IDRole() && value.canConvert<int>())
		{
			int lastID = read(_data[index.row()], index.row(), role).toInt();
			if (lastID != value.toInt())
			{
				if (!edit(_data[index.row()], value, index.row(), role))
					return false;
				_index[value.toInt()] = index.row();
				_index.erase(lastID);
				return true;
			}
		}
		if (edit(_data[index.row()], value, index.row(), role))
		{

			emit dataChanged(index, index);
			return true;
		}
		return false;

	}
	QModelIndex idToIndex(int id) const
	{
		if (!_index.contains(id))
			return QModelIndex();
		return index(_index.at(id));
	}
	template<class Iter>
	bool insertRange(int row, Iter begin, Iter end)
	{
		const auto [first, last] = std::ranges::remove_if(std::ranges::subrange(begin, end), [this](const T& val) {
			if (_index.contains(read(val, 0, IDRole()).toInt()))
				return false;
			return true;
			});
		int distance = std::distance(first, last) - 1;
		if (row > rowCount() || row < 0 || distance < 0)
		{
			return false;
		}
		beginInsertRows(QModelIndex(), row, row + distance);
		_data.insert(_data.begin() + row, first, last);
		for (size_t i = row; i <= row + distance; i++)
		{
			_index.insert({ read(_data[i],i,IDRole()).toInt(), i });
		}
		endInsertRows();

	}
	bool setData(const QModelIndex& index, T val)
	{
		if (!index.isValid() || index.row() >= rowCount() || index.row() < 0)
		{
			return false;
		}
		int newID = read(val, index.row(), IDRole()).toInt();
		int oldID = read(_data[index.row()], index.row(), IDRole()).toInt();
		if (oldID != newID)
		{
			_index[newID] = index.row();
			if (_index.contains(oldID))
				_index.erase(oldID);
		}
		_data.emplace(_data.begin() + index.row(), std::move(val));
		return true;
	}
	bool setData(int id, const QVariant& value, int role)
	{
		if (!_index.contains(id))
		{
			return false;
		}
		if (edit(_data[id],value, _index[id], role))
		{
			if(role==IDRole() && value.canConvert<int>())
			{
				if(id != value.toInt())
				{
					_index[value.toInt()] = _index[id];
					_index.erase(id);
				}
			}
			QModelIndex changed = index(_index[id]);
			emit dataChanged(changed, changed);
			return true;
		}
		return false;
	}
	int rowCount(const QModelIndex& parent = QModelIndex()) const override final
	{
		return _data.size();
	}
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override final
	{
		Q_UNUSED(parent);
		if (row < 0 || row >rowCount() || count <= 0)
			return false;
		beginInsertRows(parent, row, row + count - 1);
		if (row + count < _data.size())
		{
			for (auto i = _data.begin() + row + 1; i < _data.end(); ++i)
			{
				_index[read(*i,0,IDRole()).toInt()] += count;
			}
		}
		_data.insert(_data.begin() + row, count, std::move(create()));
		endInsertRows();
		return true;
	}

	bool insertDataList(int row, QList<T> in)
	{
		if (row < 0 || row >rowCount() || in.size() <= 0)
			return false;
		beginInsertRows(QModelIndex(), row, row + in.size() - 1);
		if (row < _data.size())
		{
			for (auto i = _data.begin() + row; i < _data.end(); ++i)
			{
				_index[read(*i, 0, IDRole()).toInt()] += in.size();
			}
		}
		for (size_t i = 0; i < in.size(); i++)
		{
			_index[read(in[i], 0, IDRole()).toInt()] = row + i;
		}
		_data.insert(_data.begin() + row, std::make_move_iterator(in.begin()),
			std::make_move_iterator(in.end()));
		endInsertRows();
		return true;
	}
	bool insertData(int row, T data)
	{
		return insertDataList(row, { std::move(data) });
	}
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override final
	{
		Q_UNUSED(parent);
		if (row<0 || row + count > rowCount() || count <= 0)
			return false;
		beginRemoveRows(parent, row, row + count - 1);
		_index.erase(read(_data[row], row, IDRole()).toInt());
		free(std::move(_data[row]));
		_data.erase(_data.begin() + row, _data.begin() + row + count);
		for (size_t i = row; i < _data.size(); i++)
		{
			_index[read(_data[i], i, IDRole()).toInt()] -= count;
		}
		endRemoveRows();
		return true;
	}
	QHash<int, QByteArray> roleNames() const
	{
		return _roles;
	}
	static constexpr int IDRole() { return 0; }
protected:
	virtual bool edit(T&,const QVariant&, int row, int role) = 0;
	virtual QVariant read(const T&, int row, int role) const = 0;
	virtual T create() { return T(); };
	virtual void free(T&&) {};
private:
	//id to row
	std::unordered_map<int, int> _index;
	QHash<int, QByteArray> _roles;
	C<T> _data;
};