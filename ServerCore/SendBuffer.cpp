#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 bufferSize)
{
	// 버퍼 사이즈 설정해주고
	_buffer.resize(bufferSize);

}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int32 len)
{
	// 예외 처리
	ASSERT_CRASH(Capacity() >= len);

	std::memcpy(_buffer.data(), data, len);

	_writeSize = len;

}
