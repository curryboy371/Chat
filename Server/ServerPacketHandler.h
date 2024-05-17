#pragma once

#include "BufferReader.h"
#include "BufferWriter.h"

enum
{
	S_TEST = 1
};




template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) : _container(container), _index(index) { }

	bool				operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { _index++; return *this; }
	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }

private:
	C& _container;
	uint16			_index;
};



template<typename T>
class PacketList
{
public:
	PacketList() : _data(nullptr), _count(0) { }
	PacketList(T* data, uint16 count) : _data(data), _count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	// ranged-base for 지원
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16		_count;
};

#pragma pack(push, 1)

// [PKT_S_TEST] [BuffsListItem][BuffsListItem]...
struct PKT_S_TEST
{

	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		/* inner dynamic data */  // 4
		uint16 victimCount;
		uint16 victimOffset;

	};

	struct VictimData
	{
		uint64 userId;
		uint32 remainHp;
	};

	/* packet header */  //4
	uint16 packetSize;
	uint16 packetId;

	/* data*/			// 14	
	uint64 id;
	uint32 hp;
	uint16 attack;

	/* dynamic data */  // 4
	uint16 buffsCount;
	uint16 buffsOffset;


};
#pragma pack(pop)


class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using VictimData = PKT_S_TEST::VictimData;
	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using VictimList = PacketList<PKT_S_TEST::VictimData>;

	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		// buffer open and writer init size 
		_sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
		_bufferWriter = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		// 이거 필요 없음?
		//PacketHeader* packetHeader = _bufferWriter.Reserve<PacketHeader>(); 필요없나? cli에서도 빠졌네

		// static data space reserve
		_pkt = _bufferWriter.Reserve<PKT_S_TEST>();
		
		// static data write
		_pkt->packetSize = 0; // to fill
		_pkt->packetId = S_TEST;

		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;

		_pkt->buffsCount = 0;  // to fill
		_pkt->buffsOffset = 0;  // to fill


	}

	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		BuffsListItem* firstbuffsListItem = _bufferWriter.Reserve<BuffsListItem>(buffCount);
		_pkt->buffsOffset = (uint64)firstbuffsListItem - (uint64)_pkt;
		_pkt->buffsCount = buffCount;
		return BuffsList(firstbuffsListItem, buffCount);
	}

	VictimList ReserveVictimList(BuffsListItem* ownerBuff, uint16 victimCount)
	{
		VictimData* firstvictimdata = _bufferWriter.Reserve<VictimData>(victimCount);
		ownerBuff->victimCount = victimCount;
		ownerBuff->victimOffset = _pkt->buffsOffset + (sizeof(*ownerBuff) * _pkt->buffsCount);

		return VictimList(firstvictimdata, victimCount);
	}

	SendBufferRef CloseAndReturn()
	{
		// calc packet size 
		_pkt->packetSize = _bufferWriter.WriteSize();

		// send packet close
		_sendBuffer->Close(_pkt->packetSize);

		return _sendBuffer;
	}

private:
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bufferWriter;

};

class ServerPacketHandler
{

public:

	//why static?
	static void HandlePacket(BYTE* buffer, int32 len);
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack, std::vector<PKT_S_TEST::BuffsListItem>& buffs);


};

