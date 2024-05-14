#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <atomic>
#include <mutex>

class MutexGuard {

public:
	explicit MutexGuard(std::recursive_mutex& mutex)
		:_guard(mutex)
	{
		// lock을 획들할때까지...
		while (!_guard.try_lock())
		{
			std::this_thread::yield();
		}
		// try_lcok에서 성공시 lock 처리
		//_guard.lock();
	}

	~MutexGuard()
	{
		_guard.unlock();
	}

    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;


private:
	std::recursive_mutex& _guard;
};


class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			_lock;
	vector<thread>	_threads;
};

