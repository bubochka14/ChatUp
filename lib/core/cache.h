#pragma once
#include <unordered_map>
#include <optional>
#include <functional>
template<class Key, class T>
class Cache
{
	explicit Cache(int capacity,
		std::function<void(T&&)> deleter = [](T&&){}) 
		:_capacity(capacity) 
		,_deleter(std::move(deleter))
	{}
	void insert(const Key& key, T&& value)
	{
		auto old = _data.extract(key);
		if (!old)
		{
			_deleter(std::move(old));
			_data.emplace(key, std::move(value));
			return;
		}
		if(_data.size()==_capacity)
	}
	void remove(const Key& key)
	{
		if (data.contains(key))
		{
			data.erase(key);
		}

	}
	std::optional<T> get(const Key& key)
	{
		if (_data.contains(key))
			return _data.at(key);
		return std::nullopt;
	}
	int capacity() const
	{
		return _capacity;
	}
private:
	std::unordered_map<Key, T> _data;
	int _capacity;
	std::function<void(T&&)> _deleter;
};