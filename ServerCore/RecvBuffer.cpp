#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 bufferSize)
	:_bufferSize(bufferSize)
{
	// 버퍼 사이즈 설정
	// Clean()함수에서 memcpy로 버퍼를 복사하는 횟수를 최소화하기 위해 버퍼 사이즈를 크게 잡아둠.
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{

		// read, write의 pos가 일치한다면 ( 읽을 버퍼가 없는 경우 ) 버퍼 초기화
		// 이미 사용된 데이터는 어차피 다시 덮어 쓰이고 사용되므로 지우진 않고 위치만 초기화
		_readPos = _writePos = 0; 
	}
	else
	{
		// 그렇지 않은 경우에는 ( 읽을 버퍼가 있는 경우 ) 
		// 버퍼의 시작 위치부터 읽어야하는 버퍼를 복사한다.
		// 이러한 복사가 실행되는 횟수를 최소화 해야함.

		// 여유공간이 버퍼 하나보다 줄어드는 경우 데이터 당기기
		if (FreeSize() < _bufferSize)
		{
			std::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
	{
		return false;
	}

	_readPos += numOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
	{
		return false;
	}

	_writePos += numOfBytes;

	return true;
}

BYTE* RecvBuffer::ReadPos()
{
	return &_buffer[_readPos];
}

BYTE* RecvBuffer::WritePos()
{
	return &_buffer[_writePos];
}

int32 RecvBuffer::DataSize()
{
	// read가능 사이즈 반환

	int32 dataSize = _writePos - _readPos;
	return dataSize;
}

int32 RecvBuffer::FreeSize()
{
	// write 가능 사이즈 반환

	int32 freeSize = _capacity - _writePos;
	return freeSize;
}
