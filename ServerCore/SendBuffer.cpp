#include "pch.h"
#include "SendBuffer.h"
#include "ThreadManager.h"


SendBufferChunk::SendBufferChunk()
	:_buffer(), _open(false), _usedSize(0)
{
}

SendBufferChunk::~SendBufferChunk()
{

}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
	{
		return nullptr;
	}

	_open = true;

	// 여기서 sharedptr 새로 만드는게 아닌지 체크.
	// 기존걸 레퍼런스 카운트 늘리는 식으로 사용해야해.
	// 실제 여기는 새로 복사해서 쓰는거 아님?
	return std::make_shared<SendBuffer>(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{

	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;

}


SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 bufferSize)
	:_owner(owner), _buffer(buffer), _allocSize(bufferSize)
{
	// 버퍼 사이즈 설정해주고
}

SendBuffer::~SendBuffer()
{

}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);

}

SendBufferRef SendBufferManager::Open(uint32 size)
{
	// 쓰레드마다 가지고 있는 자원이므로 경합없음.
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false );

	// 다 썼으면 버리고 새로 교체함.
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	std::cout << "\nFREE : ID-" << LThreadId<< " Size-" << LSendBufferChunk->FreeSize() << " Len-" << GetBufferChunkCount();

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	// 버퍼를 꺼내 씀
	{
		// WRITE_LOCK
		MutexGuard LockGuard(_mtx);

		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}

	}

	// 여유분이 없으면 새로 생성
	// 레퍼런스카운트가 0이되면 delete가 호출되지 않고 PushGlobal이 호출됨
	// 객체와 별개로 유일하게 동작해야하므로  static 함수임

	return std::shared_ptr<SendBufferChunk>(new SendBufferChunk, PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	// 버퍼에 추가
	// WRITE_LOCK
	MutexGuard LockGuard(_mtx);
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{

	// 반납한 버퍼 재사용
	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
}
