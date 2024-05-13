#pragma once


class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();


	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);


	BYTE* ReadPos();
	BYTE* WritePos();
	int32 DataSize();
	int32 FreeSize();


private:
	int32 _capacity = 0; 

	int32 _bufferSize = 0;	// 버퍼의 크기  
	int32 _readPos = 0;		//  읽은 버퍼 위치  ( write된 버퍼를 어디까지 읽었는지 )
	int32 _writePos = 0;	//  쓰여진 버퍼 위치 ( recv를 받아 입력된 위치 )
	std::vector<BYTE> _buffer;


};

