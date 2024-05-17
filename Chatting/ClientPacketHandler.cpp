#include "pch.h"
#include "ClientPacketHandler.h"



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

		bool Validate(uint32& Outsize)
		{
			Outsize += sizeof(VictimData)* victimCount;

			// dynamic data offset check
			if (victimOffset + victimCount * sizeof(VictimData) > Outsize)
			{
				return false;
			}

			return true;
		}
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

	/* valid check */
	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST); // static data 

		if (packetSize < size)
		{
			return false;
		}

		size += buffsCount * sizeof(BuffsListItem); // dynamic data

		// dynamic data offset check
		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > size)
		{
			return false;
		}


		// buffer
		// 버퍼들의 size
		PKT_S_TEST::BuffsList buffs = GetBuffsList();
		for (uint16 i = 0; i < buffsCount; ++i)
		{
			// 사이즈 누적
			if (buffs[i].Validate(size) == false)
			{
				return false;
			}
		}

		// 가변 데이터의 가변 데이터를 검증해야하므로 마지막에 check
		// packet size check
		if (size != packetSize)
		{
			//return false;
		}

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using VictimList = PacketList<PKT_S_TEST::VictimData>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);

	}

	VictimList GetVictimList(uint16 victimOffset, uint16 victimCount)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += victimOffset;
		return VictimList(reinterpret_cast<PKT_S_TEST::VictimData*>(data), victimCount);

	}
};
#pragma pack(pop)


void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BYTE tempBuffer[BUFFER_SIZE];
	std::memcpy(tempBuffer, &buffer[0], len);

	Handle_S_TEST(buffer, len);
	return;

	BufferReader bufferReader(buffer, len);

	PacketHeader header;

	bufferReader >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;

	default:
		break;
	}

}

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	// 버퍼 초기화
	BufferReader bufferReader(buffer, len);


	// 최소한 packet의 static_data 부분을 가져올 수 있는가 체크
	//if (len < sizeof(PKT_S_TEST))
	//{
	//	return;
	//}

	// header를 가져오고
	// offset o번부터의 data라서 형변환하여 사용 가능.
	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);
	//PKT_S_TEST pkt;
	//bufferReader >> pkt;

	// offset data까지 체크
	if (pkt->Validate() == false)
	{
		return;
	}

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	uint16 size = sizeof(PKT_S_TEST::BuffsList);

	std::cout << " Buff Count " << pkt->buffsCount << endl;

	for (int32 inddexBuff = 0; inddexBuff < buffs.Count(); ++inddexBuff)
	{
		PKT_S_TEST::BuffsListItem tmepbuff = buffs[inddexBuff];

		std::cout << " Buff Info " << buffs[inddexBuff].buffId << " " << buffs[inddexBuff].remainTime << endl;

		uint32 victimCnt = buffs[inddexBuff].victimCount;
		uint32 victimOffset = buffs[inddexBuff].victimOffset;

		PKT_S_TEST::VictimList victims = pkt->GetVictimList(victimOffset, victimCnt);
		
		for (int32 indexVictim = 0; indexVictim < victims.Count(); ++indexVictim)
		{
			victims[indexVictim].remainHp;
			victims[indexVictim].userId;
			cout << "id, hp" << victims[indexVictim].remainHp << " " << victims[indexVictim].userId << endl;
		}
	}
}
