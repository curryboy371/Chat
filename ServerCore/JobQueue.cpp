#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"


void JobQueue::Push(JobRef job, bool pushOnly/* = false */)
{
	// 순서 주의
	const int32 preCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	// 첫번째로 잡을 넣은 잡이면... 해당 쓰레드가 실행까지...
	if (preCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		// 이렇게 하면 실행순서에 대한 보장은 없을 것임.
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Excute();
		}
		else
		{
			// 다른 쓰레드가 실행하도록 넘김
			GGlobalQueue->Push(shared_from_this());
		}

	}
}

// 수정사항..
// 일감이 너무 몰리는경우 excute를 실행하는 thread에만 부하가 걸리게 됨.
void JobQueue::Excute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		std::vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());

		for (int32 i = 0; i < jobCount; ++i)
		{
			jobs[i]->Excute();
		}

		// 남은 일감이 0이면 ( 반복문을 실행하는 과정에 다른 job이 추가되지 않았다면
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		// 스레드가 excute에서 너무 오래 머무른경우
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;

			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘김
			GGlobalQueue->Push(shared_from_this());
			break;
		}

		// 그렇지 않은경우 다시 반복문 실행.
	}
}
