#pragma once
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include <semaphore>
#include <thread>
#include <queue>

namespace chatup 
{

	template<size_t size, class T>
	class DataPipe : public std::enable_shared_from_this<DataPipe<size,T>>
	{

	public:
		template<class DataType, bool ReadOnly>
		class PipeDataHandle
		{
		public:
			PipeDataHandle(std::shared_ptr<DataType> data, std::shared_ptr<DataPipe> pipe, size_t index)
				: m_data(std::move(data))
				, m_pipe(std::move(pipe))
				, m_index(index)
				, m_valid(true) {
			}

			PipeDataHandle(const PipeDataHandle& other) = delete;
			PipeDataHandle& operator=(const PipeDataHandle& other) = delete;

			PipeDataHandle(PipeDataHandle&& other) noexcept
			{
				m_data = std::move(other.m_data);
				m_pipe = std::move(other.m_pipe);
				m_index = other.m_index;

				m_valid = other.m_valid;
				other.m_valid = false;

			}

			PipeDataHandle& operator=(PipeDataHandle&& other) noexcept
			{
				m_data = std::move(other.m_data);
				m_pipe = std::move(other.m_pipe);
				m_index = other.m_index;

				m_valid = other.m_valid;
				other.m_valid = false;
				return *this;

			}

			bool IsValid() const
			{
				return m_valid;
			}

			DataType* operator->() {
				return m_data.get();
			}

			DataType* Get()
			{
				return m_data;
			}

			void Unmap()
			{
				if (m_valid) {
					if (ReadOnly)
						m_pipe->UnmapReading(m_index);
					else
						m_pipe->UnmapUploading(m_index);
				}
				m_data.reset();
			}

			~PipeDataHandle()
			{
				Unmap();
			}

		private:
			std::shared_ptr<DataType> m_data;
			std::shared_ptr<DataPipe> m_pipe;
			size_t m_index;
			bool m_valid;
		};
		class UploadListenerHandle
		{
		public:
			UploadListenerHandle(const UploadListenerHandle& other) = delete;
			UploadListenerHandle& operator=(const UploadListenerHandle& other) = delete;

			UploadListenerHandle(std::shared_ptr<DataPipe> pipe, int listenerIndex) : m_pipe(std::move(pipe)),
				m_listenerIndex(listenerIndex), m_isListening(true)
			{
			}
			UploadListenerHandle(UploadListenerHandle&& other) noexcept
			{
				m_pipe = std::move(other.m_pipe);
				m_listenerIndex = other.m_listenerIndex;
				m_isListening = other.m_isListening;
				other.m_isListening = false;
			}
			UploadListenerHandle& operator=(UploadListenerHandle&& other) noexcept
			{
				m_pipe = std::move(other.m_pipe);
				m_listenerIndex = other.m_listenerIndex;
				m_isListening = other.m_isListening;
				other.m_isListening = false;
				return *this;
			}
			~UploadListenerHandle()
			{
				Unsubscribe();
			}

			std::shared_ptr<DataPipe> GetPipe() { return m_pipe; }
			bool IsListening() const { return m_isListening; }
			void Unsubscribe() {
				if (m_isListening)
					m_pipe->RemoveListener(m_listenerIndex);
			}
		private:
			std::shared_ptr<DataPipe> m_pipe;
			int m_listenerIndex;
			bool m_isListening;

			
		};
		using WritableDataHandle = PipeDataHandle<T, false>;
		using ReadonlyDataHandle = PipeDataHandle<const T, true>;
		using DataCallback = std::function<void(ReadonlyDataHandle)>;
		using Constructor = std::function<T* ()>;
		using Deleter = std::function<void(T*)>;

		DataPipe(const Constructor& constructor, const Deleter& deleter)
			:sem(size) {

			for (size_t i = 0; i < size; i++)
			{
				auto* temp = constructor();
				subpipes[i].ptr = std::shared_ptr<T>(temp, deleter);
			}
		}

		explicit DataPipe(bool constructSubPipes = true)
			:sem(size) {

			if (constructSubPipes)
			{
				for (size_t i = 0; i < size; i++)
					subpipes[i].ptr = std::make_shared<T>();
			}else 
				for (size_t i = 0; i < size; i++)
					subpipes[i].ptr = nullptr;
		}

		//unsafe
		std::shared_ptr<T> ForceGetSubPipeData(size_t index) const
		{
			std::lock_guard guard(reader_mutex);
			return subpipes[index].ptr;
		}

		//unsafe
		void ForceSetSubPipeData(size_t index, std::shared_ptr<T> ptr)
		{
			std::lock_guard guard(reader_mutex);
			subpipes[index].ptr = ptr;
		}

		WritableDataHandle HoldForUploading(){
			sem.acquire();
			std::lock_guard guard(reader_mutex);
			return GetFreeSubPipeForWriting();

		}
		WritableDataHandle TryHoldForUploading(std::chrono::duration<float> duration
			= std::chrono::milliseconds(500))
		{
			if(sem.try_acquire_for(duration))
			{
				std::lock_guard guard(reader_mutex);
				return GetFreeSubPipeForWriting();
			}
			return ConstructEmptyUploadHandle();
		}
		UploadListenerHandle AddUploadListener(DataCallback c)
		{

			UploadListenerHandle handle{this->shared_from_this(),++listenerFreeIndex };
			{
				std::lock_guard guard(reader_mutex);
				listeners.emplace(handle.m_listenerIndex, std::move(c));
			}
			return handle;
		}

		static constexpr size_t getSize()
		{
			return size;
		}
	private:
		void RemoveListener(int index)
		{
			std::lock_guard guard(reader_mutex); //!
			removeListenerQueue.emplace(index);
			//listeners.erase(index);

		}
		void UnmapUploading(size_t index, bool notifyReaders){
			{
				std::lock_guard guard(reader_mutex);
				while (!addListenerQueue.empty())
				{
					std::pair<DataCallback, int> top = addListenerQueue.front();
					addListenerQueue.pop();
					listeners.emplace(top.second, std::move(top.first));
				}
				while (!removeListenerQueue.empty())
				{
					int top = removeListenerQueue.front();
					removeListenerQueue.pop();
					listeners.erase(top);
				}
				if (!notifyReaders || listeners.empty())
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
		void UnmapReading(size_t index)
		{
			if (--subpipes[index].readings == 0 && !subpipes[index].isFree)
			{
				subpipes[index].isFree = true;

				sem.release();

			}
		}
		WritableDataHandle GetFreeSubPipeForWriting()
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
			bool m_isCapturedForWriting = true;
			size_t m_currentReadings = 0;
			std::shared_ptr<T> m_data;
		};
		WritableDataHandle ConstructEmptyUploadHandle()
		{
			return WritableDataHandle{ nullptr,this,0,false };

		}

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
