#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "SendBuffer.h"

ThreadManager* GThreadManager = nullptr;
SendBufferManager* GSendBufferManager = nullptr;


CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();
	GSendBufferManager = new SendBufferManager;


	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;
	delete GSendBufferManager;

	SocketUtils::Clear();

}
