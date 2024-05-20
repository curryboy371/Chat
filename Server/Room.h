#pragma once
#include "Job.h"
#include "SendBuffer.h"

// room은 single thread처럼..

class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// multi thread 환경에서는 job으로 접근
	//void PushJob(JobRef job) { _jobs.Push(job); }
	void FlushJob();
	

	template<typename Ret, typename... Args>
	void PushJob(Ret(*Func)(Args...), Args... args)
	{
		auto job = std::make_shared<FuncJob2<Ret, Args...>>(Func, args...);
		_jobs.Push(job);
	}

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::* memFunc)(Args...), Args... args)
	{
		auto job = std::make_shared<MemberJob2<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}
private:
	//std::recursive_mutex _mtx;
	std::map<uint64, PlayerRef> _players;
	JobQueue _jobs;
};


extern Room GRoom;


class EnterJob : public IJob
{
public:
	EnterJob(Room& room, PlayerRef player)
		:_room(room), _player(player)
	{

	}

	virtual void Excute() override
	{
		_room.Enter(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};


class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer)
		:_room(room), _sendBuffer(sendBuffer)
	{

	}

	virtual void Excute() override
	{
		_room.Broadcast(_sendBuffer);
	}

public:
	Room& _room;
	SendBufferRef _sendBuffer;
};