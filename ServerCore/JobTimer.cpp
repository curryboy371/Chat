#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

#include "ThreadManager.h"

void JobTimer::Reservc(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job)
{
	const uint64 tickCount = GetTickCount64();
	const uint64 excuteTick = tickCount + tickAfter;

	JobData* jobData = new JobData(owner, job);

	MutexGuard LockGuard(_mtx);


	_items.push(TimerItem{ excuteTick, jobData });


}

void JobTimer::Distribute(uint64 now)
{
	// 한번에 한 thread만...
	if (_distributing.exchange(true) == true)
	{
		// 이미 distributing이 true여서 실행되고 있는 경우...
		// 한번에 하나의 thread만 실행하기 위해 return
		return;
	}

	std::vector<TimerItem> items;
	{
		MutexGuard LockGuard(_mtx);

		while (_items.empty() == false)
		{
			const TimerItem& timerItem = _items.top();

			if (now < timerItem.excuteTick)
			{
				break;
			}

			items.push_back(timerItem);
			_items.pop();

		}
	}


	for (TimerItem& item : items)
	{
		if (JobQueueRef owner = item.jobData->owner.lock())
		{
			bool pushOnly = true; // 실행중인 job이 없더라도 예약이므로 즉시 실행시키지 않음
			owner->Push(item.jobData->job, pushOnly);

		}
		// data 껍데기 소멸..
		delete item.jobData;
	}

	// 종료
	_distributing.store(false);

}

void JobTimer::Clear()
{
	MutexGuard LockGuard(_mtx);

	while (_items.empty() == false)
	{
		const TimerItem& timerItem = _items.top();
		// data 껍데기 소멸..
		delete timerItem.jobData;
		_items.pop();
	}
}
