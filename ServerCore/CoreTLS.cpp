#include "pch.h"
#include "CoreTLS.h"

thread_local unsigned int LThreadId = 0;
thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;

