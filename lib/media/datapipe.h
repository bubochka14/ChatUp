#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <shared_mutex>
#include <mutex>
#include <semaphore>
#include <thread>
#include <queue>
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
			std::lock_guard guard(reader_mutex);
			for (size_t i = 0; i < size; i++)
			{
				subpipes[i].ptr = std::shared_ptr<T>(constructor(), [deleter](T* t) {deleter(t); });
			}
		}
		//unsafe
		std::shared_ptr<T> storedData(size_t index)
		{
			std::lock_guard guard(reader_mutex);
			return subpipes[index].ptr;
		}
		void lock()
		{
			reader_mutex.unlock();
		}
		void unlock()
		{
			reader_mutex.lock();
		}
		PipeData holdForWriting(){
			sem.acquire();
			std::lock_guard guard(reader_mutex);
			return getFree();

		}
		std::optional<PipeData> tryHoldForWriting(std::chrono::duration<float> duration
			= std::chrono::milliseconds(500))
		{
			if(sem.try_acquire_for(duration))
			{
				std::lock_guard guard(reader_mutex);
				return getFree();
			}
			return std::nullopt;
		}
		int onDataChanged(DataCallback c)
		{
			std::lock_guard guard (reader_mutex);
			addListenerQueue.emplace(std::move(c), ++listenerFreeIndex);
			//listeners[++listenerFreeIndex] = c;
			return listenerFreeIndex;
		}
		int listenerCount()
		{
			std::lock_guard guard(reader_mutex);
			return listeners.size();
		}
		void removeListener(int index)
		{
			std::lock_guard guard(reader_mutex);
			removeListenerQueue.emplace(index);
			//listeners.erase(index);

		}
		void unmapWriting(size_t index, bool notifyReaders)
		{
			{
				std::lock_guard guard(reader_mutex);
				while (addListenerQueue.size())
				{
					std::pair<DataCallback, int> top = addListenerQueue.front();
					addListenerQueue.pop();
					listeners.emplace(top.second, std::move(top.first));
				}
				while (removeListenerQueue.size())
				{
					int top = removeListenerQueue.front();
					removeListenerQueue.pop();
					listeners.erase(top);
				}
				if (!notifyReaders || !listeners.size())
				{
					subpipes[index].isFree = true;
					sem.release();
					return;
				}
				subpipes[index].readings = listeners.size();
			}
			for (auto& l : listeners)
			{
				l.second(subpipes[index].ptr, index);
			}

		}
		void unmapReading(size_t index)
		{
			std::lock_guard guard(reader_mutex);

			if (--subpipes[index].readings ==0 && !subpipes[index].isFree)
			{
				subpipes[index].isFree = true;

				sem.release();

			}
		}
		static constexpr size_t getSize()
		{
			return size;
		}
	private:
		PipeData getFree()
		{
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
		std::recursive_mutex reader_mutex;
		std::counting_semaphore<size> sem;
		std::array<SubPipe, size> subpipes;
		std::queue<int> removeListenerQueue;
		std::queue<std::pair<DataCallback, int>> addListenerQueue;
		inline static int listenerFreeIndex = 0;
	};

}