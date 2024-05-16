#pragma once

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{


public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
	int32 GetSessionCount();
private:
	//USE_LOCK
	std::recursive_mutex _mtx;

	//std::set<GameSessionRef> _sessions;
	std::unordered_map<uint32, GameSessionRef> _sessions;

};

extern GameSessionManager GSessionManager;