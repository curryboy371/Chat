#include "pch.h"
#include "ThreadManager.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

ThreadManager::ThreadManager()
{
	// MainThread

	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void(void)> callback)
{
	LockGuard gurad(_lock);

	_threads.push_back(thread([=]()

		{
			InitTLS();
			callback();
			DestroyTLS();
		}

	));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}
