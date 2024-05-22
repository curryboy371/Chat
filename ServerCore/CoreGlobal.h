#pragma once

extern class ThreadManager* GThreadManager;
extern class SendBufferManager* GSendBufferManager;
extern class GlobalQueue* GGlobalQueue;
extern class JobTimer* GJobTimer;
class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();
};

