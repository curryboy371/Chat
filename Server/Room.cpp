#include "pch.h"
#include "Room.h"
#include "ThreadManager.h"
#include "GameSession.h"
#include "Player.h"

Room GRoom;

void Room::Enter(PlayerRef player)
{
	//MutexGuard LockGuard(_mtx);

	_players[player->playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	//MutexGuard LockGuard(_mtx);
	_players.erase(player->playerId);

}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	//MutexGuard LockGuard(_mtx);

	for (auto& p : _players)
	{
		p.second->ownerSession->Send(sendBuffer);
	}
}

void Room::FlushJob()
{
	while (true)
	{
		JobRef job = _jobs.Pop();
		if (job == nullptr)
		{
			break;
		}

		job->Excute();
	}
}
