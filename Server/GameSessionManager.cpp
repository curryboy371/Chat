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
	_sessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);
	_sessions.erase(session);

}

void GameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);

	for (GameSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}


}

int32 GameSessionManager::GetSessionCount()
{
	return _sessions.size();
}
