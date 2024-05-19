#pragma once
class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

private:
	std::recursive_mutex _mtx;

	std::map<uint64, PlayerRef> _players;
};


extern Room GRoom;

