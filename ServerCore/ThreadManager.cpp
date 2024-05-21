#include "pch.h"
#include "ThreadManager.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

#include "GlobalQueue.h"

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

void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();
		if (now > LEndTickCount)
		{
			break;
		}

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (jobQueue == nullptr)
		{
			break;
		}

		jobQueue->Excute();

	}

}
