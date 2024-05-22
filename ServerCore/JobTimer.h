#pragma once


// Job
struct JobData
{
	JobData(std::weak_ptr<class JobQueue> owner, JobRef job) : owner(owner), job(job)
	{

	}

	std::weak_ptr<class JobQueue> owner;
	JobRef job;
};


// JobData, 예약 시간 관리 구조체
struct TimerItem
{
	bool operator < (const TimerItem& other) const
	{
		return excuteTick > other.excuteTick;
	}

	uint64 excuteTick = 0;
	JobData* jobData = nullptr;
};


// 우선순위 큐로 잡 예약시스템을 관리함
class JobTimer
{

public:
	void Reservc(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void Distribute(uint64 now);
	void Clear();


private:
	std::recursive_mutex _mtx;

	std::priority_queue<TimerItem> _items;
	std::atomic<bool> _distributing = false;
	
};

