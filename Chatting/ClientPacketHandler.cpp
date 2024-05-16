#include "pch.h"
#include "ClientPacketHandler.h"
void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{

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
	BufferReader bufferReader(buffer, len);
	PacketHeader header;
	bufferReader >> header; // PacketHeader 크기만큼 pos를 이동, header data 넣어줌.

	uint64 id = 0;
	uint32 hp = 0;
	uint16 attack = 0;
	bufferReader >> id >> hp >> attack; // 또 자료형 크기만큼 pos 이동... 이거 왜한거????

	std::vector<BuffData> buffs;
	uint16 buffSize = 0;

	bufferReader >> buffSize;

	BuffData tempData;
	for (uint16 i = 0; i < buffSize; ++i)
	{
		bufferReader >> tempData.buffId >> tempData.remainTime;
		buffs.push_back(tempData);
	}

	for (auto& data : buffs)
	{
		cout << "\n buff Info : " <<  data.buffId << " " << data.remainTime;
	}
}
