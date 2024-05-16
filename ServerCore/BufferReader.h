#pragma once


class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~BufferReader();


	BYTE* Buffer() { return _buffer; }
	uint32 Size() { return _size; }
	uint32 ReadSize() { return _pos; }
	uint32 FreeSize() { return _size - _pos; }


	template<typename T>
	bool Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool Peek(void* dest, uint32 len);


	template<typename T>
	bool Read(T* dest) { return Read(dest, sizeof(T)); }
	bool Read(void* dest, uint32 len);

	// OUT 안되는데??
	template<typename T>
	BufferReader& operator>>(T&& dest);

	template<typename T>
	BufferReader& operator>>(T& dest);

	//template<typename T>
	//BufferReader& operator>>(OUT T&& dest);


private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _pos = 0;
};


// 이거는 packetheader를 벗겨내는데 쓰는듯
// 나머지 data들도 벗겨내는건 가능하긴한데 data가 자료형 하나로 오는게 아닐테니 이러한 경우는 Read로 복사해서 사용하는 것으로 보임
// 근데 이 오퍼레이터는 템플릿 함수로 해서...size check를 컴파일 타임에도 가능하게 할 수 있지 않을까?
template<typename T>
inline BufferReader& BufferReader::operator>>(T& dest)
{
	// 자료형 크기만큼 pos를 이동시킴
	// 왜냐면..dest에 data를 빼주는거...

	//using DataType = std::remove_reference_t<T>;

	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;

}

template<typename T>
inline BufferReader& BufferReader::operator>>(T&& dest)
{
	// 자료형 크기만큼 pos를 이동시킴
	// 왜냐면..dest에 data를 빼주는거...

	//using DataType = std::remove_reference_t<T>;

	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;

}