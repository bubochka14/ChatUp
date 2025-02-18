#pragma once
#include "functional"
#include "mutex"
#include "condition_variable"
#include "queue"
#include "vector"
#include "atomic"
#include "core_include.h"
#include "chrono"
#include <qscopeguard.h>
class CC_CORE_EXPORT TaskQueue
{
public:
	using clock = std::chrono::steady_clock;
	struct Task {
		clock::time_point time;
		std::function<void()> func;
		size_t priority = 0;
		bool operator>(const Task& other) const {
			if (priority == other.priority)
				return time < other.time;
			return priority > other.priority;
		}
		bool operator<(const Task& other) const {
			if(priority == other.priority)
				return time > other.time; 
			return priority < other.priority;
		}
	};
	TaskQueue(size_t threadCount = 1);
	template <class F, class... Args>
	void schedule(clock::duration delay, F&& f, Args &&...args) noexcept
	{
		return schedule(clock::now() + delay, std::forward<F>(f), std::forward<Args>(args)...);
	}
	template <class F, class... Args>
	void enqueue(F&& f, Args &&...args) noexcept{
		return schedule(clock::now(), std::forward<F>(f), std::forward<Args>(args)...);
	}
	template <class F, class... Args>
	void schedule(clock::time_point time, F&& f, Args &&...args, size_t priority = 0) noexcept
	{
		std::lock_guard guard(mutex);
		auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		mTasks.push({time,
			[task = std::move(bound)]() { return (task)(); },
			priority});
		mTasksCondition.notify_one();
	}
	~TaskQueue();
private:

	using taskQueue = std::priority_queue<Task, std::deque<Task>, std::greater<Task>>;

	bool runOne();
	std::function<void()> dequeue(); // returns null function if joining
	void threadFunc();
	std::mutex mutex, threadsMutex;
	std::condition_variable mWaitingCondition,
		mTasksCondition;
	taskQueue mTasks;
	//std::queue<task> readBuffer;
	std::vector<std::thread> threads;
	std::atomic<bool> active;
	std::atomic<bool> mJoining = false;


};