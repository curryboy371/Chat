#pragma once

#include <vector>
#include <array>

class SendBufferChunk;

class SendBuffer : public std::enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 bufferSize);
	~SendBuffer();


	BYTE* Buffer() { return _buffer; }
	int32 WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

	//int32 Capacity() { return static_cast<int32>(_buffer.size()); }
	//void CopyData(void* data, int32 len);

private:
	//std::vector<BYTE> _buffer;
	BYTE* _buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner;
};


class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		//SEND_BUFFER_CHUNK_SIZE = 0x1000
		SEND_BUFFER_CHUNK_SIZE = 4096
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	SendBufferRef Open(uint32 allocSize);
	void Close(uint32 writeSize);

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	//uint32 FreeSize() { return static_cast<uint32>(SEND_BUFFER_CHUNK_SIZE) - _usedSize; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	std::array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer;
	//BYTE _buffer[SEND_BUFFER_CHUNK_SIZE];
	bool _open = false;
	uint32 _usedSize = 0;

};


class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);


public:
	int32 GetBufferChunkCount() { return static_cast<int32>(_sendBufferChunks.size()); }
private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer); // shared potr ref count 이 되면 이 함수를 호출하게 하여 static 함수.

	

private:
	std::recursive_mutex _mtx;

	std::vector<SendBufferChunkRef> _sendBufferChunks;

};