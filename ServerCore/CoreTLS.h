#pragma once

extern thread_local unsigned int LThreadId;

extern thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
extern thread_local class JobQueue* LCurrentJobQueue;

extern thread_local unsigned long long LEndTickCount;
