#pragma once

// 한 스레드가 너무 오래 job을 실행하게 두지 않도록
// 전역 qeueue를 두어 관리함.
// global queue에 push, pop은 싱글 스레드처럼 동작하지만, 해당 함수 내부에서 multi thread에서 lock을 잡게 동작함.
// 따라서 멀티스레드 환경에서 안전할 것임.
class GlobalQueue
{

public:
	GlobalQueue();
	~GlobalQueue();

	void Push(JobQueueRef jobQueue);
	JobQueueRef Pop();

private:
	LockQueue<JobQueueRef> _jobQueues; // 실행되지 못한 Job은 여기서 관리함
};

