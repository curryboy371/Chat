#pragma once
#include "Job.h"
#include "LockQueue.h"

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		// 왜 std move를 썼는가
		Push(std::make_shared<Job>(std::move(callback)));
	}


	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(std::make_shared<Job>(owner, memFunc, std::forward<Args>(args)...));
	}

	void ClearJobs() { _jobs.Clear(); }


public:
	void Excute();

private:
	void Push(JobRef&& job);

	virtual void FlushJob() abstract;

protected:
	LockQueue<JobRef> _jobs;
	std::atomic<int32> _jobCount = 0;

private:
};

