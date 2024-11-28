#pragma once
#include "functional"
#include "mutex"
#include "condition_variable"
#include "queue"
#include "vector"
#include "atomic"
#include "core_include.h"

class CC_CORE_EXPORT TaskQueue
{
	using task = std::function<void()>;
public:
	TaskQueue(size_t threadCount = 1);
	void pushTask(const task& t);
	void pushTask(task&& t);
	~TaskQueue();
private:
	void threadFunc();
	std::mutex mutex;
	std::condition_variable condvar;
	std::queue<task> incomingTasks;
	std::queue<task> readBuffer;
	std::vector<std::thread> threads;
	std::atomic<bool> active;

};