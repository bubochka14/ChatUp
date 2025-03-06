#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <shared_mutex>
#include <semaphore>
#include <thread>

#include <condition_variable>
#include "media_include.h"
namespace Media 
{
	//threadsafe
	template<int size, class T>
	class DataPipe
	{

	public:
		using DataCallback = std::function<void(std::shared_ptr<T>,size_t index)>;
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
		DataPipe(bool constructSubpipes = true)
			:sem(size)
		{
			if (constructSubpipes)
			{
				for (size_t i = 0; i < size; i++)
					subpipes[i].ptr = std::make_shared<T>();
			}else 
				for (size_t i = 0; i < size; i++)
					subpipes[i].ptr = nullptr;

		}
		void reset(const Constructor& constructor, const Deleter& deleter)
		{
			std::lock_guard<std::mutex>guard(reader_mutex);
			for (size_t i = 0; i < size; i++)
			{
				subpipes[i].ptr = std::shared_ptr<T>(constructor(), [deleter](T* t) {deleter(t); });
			}
		}
		//unsafe
		std::shared_ptr<T> storedData(size_t index)
		{
			std::lock_guard<std::mutex>guard(reader_mutex);
			return subpipes[index].ptr;
		}
		PipeData holdForWriting(){
			sem.acquire();
			return getFree();

		}
		std::optional<PipeData> tryHoldForWriting()
		{
			if(sem.try_acquire())
				return getFree();
			return std::nullopt;
		}
		int onDataChanged(const DataCallback& c)
		{
			std::lock_guard<std::mutex>guard (reader_mutex);
			listeners[++listenerFreeIndex] = c;
			return listenerFreeIndex;
		}
		void removeListener(int index)
		{
			std::lock_guard<std::mutex>guard(reader_mutex);
			listeners.erase(index);

		}
		void unmapWriting(size_t index, bool notifyReaders)
		{
			if (!notifyReaders || !listeners.size())
			{
				subpipes[index].isFree = true;
				sem.release();
				return;
			}
			subpipes[index].readings = listeners.size();
			for (auto& l : listeners)
			{
				l.second(subpipes[index].ptr, index);
			}

		}
		void unmapReading(size_t index)
		{
			std::lock_guard<std::mutex>guard(reader_mutex);

			if (--subpipes[index].readings ==0 && !subpipes[index].isFree)
			{
				subpipes[index].isFree = true;

				sem.release();

			}
		}

	private:
		PipeData getFree()
		{
			std::lock_guard<std::mutex>guard(reader_mutex);

			for (size_t i = 0; i < subpipes.size(); i++)
			{
				if (subpipes[i].isFree)
				{
					subpipes[i].isFree = false;
					return PipeData(subpipes[i].ptr, i);
				}
			}
			//should never happen
			throw std::logic_error("Get free pipe error");
		}
		struct SubPipe
		{
			bool isFree = true;
			std::shared_ptr<T> ptr;
			size_t readings = 0;
		};
		std::weak_ptr<T> make_weak_ptr(std::shared_ptr<T> ptr) { return ptr; }
		std::map<int,DataCallback> listeners;
		Constructor constructor;
		Deleter     deleter;
		std::mutex reader_mutex;
		std::counting_semaphore<size> sem;
		std::array<SubPipe, size> subpipes;
		inline static int listenerFreeIndex = 0;
	};

}