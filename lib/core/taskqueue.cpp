#include "taskqueue.h"
TaskQueue::TaskQueue(size_t threadCount)
	:threads(threadCount)
	,active(true)
{
	for (auto& i : threads)
	{
		i = std::thread(&TaskQueue::threadFunc, this);
	}
}
void TaskQueue::pushTask(const task& t)
{
	std::lock_guard guard(mutex);
	incomingTasks.push(t);
	condvar.notify_one();
}
void TaskQueue::pushTask(task&& t)
{
	std::lock_guard<std::mutex> guard(mutex);
	incomingTasks.emplace(std::move(t));
	condvar.notify_one();

}
TaskQueue::~TaskQueue()
{
	std::lock_guard g(mutex);
	active = false;
	condvar.notify_all();
	for (auto& i : threads)
	{
		if(i.joinable())
		i.join();
	}
}

void TaskQueue::threadFunc()
{
	while (1)
	{
		{
			std::unique_lock lock(mutex);
			condvar.wait(lock, [this]() {return !incomingTasks.empty() || !active; });
			if (!active)
				return;
			std::swap(readBuffer, incomingTasks);
		}
		for (; !readBuffer.empty(); readBuffer.pop())
		{
			readBuffer.front()();
		}
	}
	
}