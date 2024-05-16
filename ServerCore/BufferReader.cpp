#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader()
{


}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 pos)
	:_buffer(buffer), _size(size), _pos(pos)
{

}

BufferReader::~BufferReader()
{

}

bool BufferReader::Peek(void* dest, uint32 len)
{

	// 읽을 cusor pos 부터 남은 데이터보다 len가 크면 안됨.
	// dest len 만큼 buffer에.... 새로 복사????
	if (FreeSize() < len)
	{
		return false;
	}

	BYTE* temp = &_buffer[_pos];

	cout << temp << endl;
	std::memcpy(dest, &_buffer[_pos], len);

	BYTE* casttemp = reinterpret_cast<BYTE*>(dest);

	cout << casttemp << endl;

	return true;
}

bool BufferReader::Read(void* dest, uint32 len)
{
	// dest len 만큼 새로 복사해내고...
	if (Peek(dest, len) == false)
	{
		return false;
	}

	// len만큼 커서를 이동시킴...

	// 이렇게 하는 이유는...??
	// 읽는 방식이 2개임?  read와 >> 


	_pos += len;
	return true;
}
