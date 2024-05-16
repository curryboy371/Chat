#include "pch.h"
#include "GameSessionManager.h"
#include "ThreadManager.h"
#include "GameSession.h"


//#include "GameSession.h"
GameSessionManager GSessionManager;

void GameSessionManager::Add(GameSessionRef session)
{
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);
	uint32 index = static_cast<uint32>(_sessions.size());
	session->SetSessionID(index);
	_sessions.emplace(index, session);
	std::cout << "\nAddSession ID " << session->GetSessionID();

}

void GameSessionManager::Remove(GameSessionRef session)
{
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);
	std::cout << "\nRemoveSession ID " << session->GetSessionID();
	_sessions.erase(session->GetSessionID());
}

void GameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);

	for (auto& sessionPair : _sessions)
	{
		auto& sessionID = sessionPair.first;
		auto& sessionPtr = sessionPair.second;

		sessionPtr->Send(sendBuffer);
	}
}

int32 GameSessionManager::GetSessionCount()
{
	return static_cast<int32>(_sessions.size());
}
