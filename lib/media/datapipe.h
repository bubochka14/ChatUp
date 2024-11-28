#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <shared_mutex>
#include <semaphore>
#include <condition_variable>
#include "media_include.h"
namespace media {
	template<int size, class T>
	class CC_MEDIA_EXPORT DataPipe
	{

	public:
		using DataCallback = std::function<void(std::weak_ptr<T>,size_t index)>;
		using Constructor = std::function<T*()>;
		using Deleter = std::function<void(T*)>;

		struct PipeData
		{
			std::shared_ptr<T> ptr;
			size_t subpipe;
			T* operator->() {
				return ptr.get();
			}
		};
		DataPipe(const Constructor& constructor, const Deleter& deleter)
			:sem(size)
		{
			for (size_t i = 0; i < size; i++)
			{
				auto temp = constructor();
				subpipes[i].ptr = std::shared_ptr<T>(temp, deleter);
			}
		}
		DataPipe()
			:DataPipe([]() {return nullptr; }, [](T*) {})
		{}
		void reset(const Constructor& constructor, const Deleter& deleter)
		{
			for (size_t i = 0; i < size; i++)
			{
				subpipes[i].ptr = std::shared_ptr<T>(constructor(), [deleter](T* t) {deleter(t); });
			}
		}
		//unsafe
		std::shared_ptr<T> storedData(size_t index)
		{
			return subpipes[index].ptr;
		}
		PipeData holdForWriting(){
			sem.acquire();
			for (size_t i = 0; i < subpipes.size(); i++)
			{
				if (subpipes[i].isFree)
				{
					subpipes[i].isFree = false;
					return PipeData(subpipes[i].ptr, i);
				}
			}
		}
		void onDataChanged(const DataCallback& c)
		{
			std::lock_guard<std::mutex>guard (reader_mutex);
			listeners.push_back(c);
		}
		void unmapWriting(size_t index, bool notifyReaders)
		{
			std::lock_guard<std::mutex>guard(reader_mutex);

			if (!notifyReaders || !listeners.size())
			{
				subpipes[index].isFree = true;
				sem.release();
				return;
			}
			subpipes[index].readings = listeners.size();
			for (size_t i = 0; i < listeners.size(); i++)
			{
				listeners[i](std::weak_ptr(subpipes[index].ptr), index);
			}

		}
		void unmapReading(size_t index)
		{
			if (--subpipes[index].readings ==0)
			{
				subpipes[index].isFree = true;
				sem.release();
			}
		}

	private:
		struct SubPipe
		{
			std::shared_ptr<T> ptr;
			bool isFree = true;
			size_t readings = 0;
		};
		std::weak_ptr<T> make_weak_ptr(std::shared_ptr<T> ptr) { return ptr; }
		std::vector<DataCallback> listeners;
		Constructor constructor;
		Deleter     deleter;
		std::counting_semaphore<size> sem;
		std::mutex reader_mutex;
		std::array<SubPipe, size> subpipes;
		
	};

}