#pragma once
#include "SendBuffer.h"
#include "Job.h"
#include "JobQueue.h"

// room은 single thread처럼..

class Room : public JobQueue
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// multi thread 환경에서는 job으로 접근
	//void PushJob(JobRef job) { _jobs.Push(job); }
	virtual void FlushJob() override;

private:
	//std::recursive_mutex _mtx;
	std::map<uint64, PlayerRef> _players;
};


extern std::shared_ptr<Room> GRoom;


//class EnterJob : public Job
//{
//public:
//	EnterJob(Room& room, PlayerRef player)
//		:_room(room), _player(player)
//	{
//
//	}
//
//	virtual void Excute() override
//	{
//		_room.Enter(_player);
//	}
//
//public:
//	Room& _room;
//	PlayerRef _player;
//};
//
//
//class BroadcastJob : public Job
//{
//public:
//	BroadcastJob(Room& room, SendBufferRef sendBuffer)
//		:_room(room), _sendBuffer(sendBuffer)
//	{
//
//	}
//
//	virtual void Excute() override
//	{
//		_room.Broadcast(_sendBuffer);
//	}
//
//public:
//	Room& _room;
//	SendBufferRef _sendBuffer;
//};