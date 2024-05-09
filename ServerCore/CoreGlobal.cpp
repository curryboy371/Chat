#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager();

	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager;

	SocketUtils::Clear();

}
