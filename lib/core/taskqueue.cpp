#include "taskqueue.h"

TaskQueue::TaskQueue(size_t threadCount)
	:threads(threadCount)
	,active(true)
{
	std::lock_guard g(threadsMutex);
	for (auto& i : threads)
		i = std::thread(&TaskQueue::threadFunc, this);
}
std::function<void()> TaskQueue::dequeue() {
	std::optional<clock::time_point> time;
	while (true) {
		std::unique_lock lock(mutex);
		if (time)
			mTasksCondition.wait_until(lock, *time);
		else
			mTasksCondition.wait(lock, [this]() {return !mTasks.empty() || mJoining; });
		if (mJoining)
			break;
		time = mTasks.top().time;
		if (*time <= clock::now()) {
			auto func = std::move(mTasks.top().func);
			mTasks.pop();
			return func;
		}
		//mWaitingCondition.notify_all();

	}
	return nullptr;
}

bool TaskQueue::runOne() {
	if (auto task = dequeue()) {
		task();
		return true;
	}
	return false;
}
TaskQueue::~TaskQueue()
{
	{
		std::lock_guard g(mutex);
		mJoining = true;
		mTasksCondition.notify_all();
	}
	std::lock_guard g(threadsMutex);
	for (auto& i : threads)
	{
		if(i.joinable())
			i.join();
		threads.clear();

	}
}

void TaskQueue::threadFunc()
{
	while (runOne()) {
	}
	
}