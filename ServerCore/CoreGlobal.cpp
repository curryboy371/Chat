#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
	GSendBufferManager = new SendBufferManager();
	GGlobalQueue = new GlobalQueue();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GSendBufferManager;
	delete GGlobalQueue;

	SocketUtils::Clear();

}
