#include "pch.h"
#include "ServerPacketHandler.h"

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{



}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, std::vector<BuffData>& buffs)
{
    SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);

    BufferWriter bufferWriter(sendBuffer->Buffer(), sendBuffer->AllocSize());

    PacketHeader* header = bufferWriter.Reserve<PacketHeader>();

    bufferWriter << id << hp << attack;

    // 가변
    bufferWriter << static_cast<uint16>(buffs.size());

    for (BuffData buff : buffs)
    {
        bufferWriter << buff.buffId << buff.remainTime;
    }

    header->size = bufferWriter.WriteSize();
    header->id = S_TEST;

    sendBuffer->Close(bufferWriter.WriteSize());

    return sendBuffer;
}
