#include "pch.h"
#include "pch.h"
#include "CoreTLS.h"

thread_local unsigned int LThreadId = 0;

thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
thread_local class JobQueue* LCurrentJobQueue = nullptr;
thread_local unsigned long long LEndTickCount = 0;


