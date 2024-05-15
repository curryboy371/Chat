#pragma once

#include <iostream>
extern thread_local unsigned int LThreadId;
extern thread_local std::shared_ptr<class SendBufferChunk> LSendBufferChunk;
