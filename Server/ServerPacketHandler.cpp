#include "pch.h"
#include "ServerPacketHandler.h"

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{



}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, std::vector<PKT_S_TEST::BuffsListItem>& buffs)
{
    SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);

    BufferWriter bufferWriter(sendBuffer->Buffer(), sendBuffer->AllocSize());

    PacketHeader* header = bufferWriter.Reserve<PacketHeader>();

    int32 curSize = bufferWriter.WriteSize();

    bufferWriter << id << hp << attack;

    curSize = bufferWriter.WriteSize();
    // dynamic data

    // count
    bufferWriter << static_cast<uint16>(buffs.size());

    // offset
    uint16 offset = static_cast<uint16>(bufferWriter.WriteSize()) + sizeof(uint16);
    bufferWriter << offset;

    curSize = bufferWriter.WriteSize();
    for (PKT_S_TEST::BuffsListItem buff : buffs)
    {
        bufferWriter << buff.buffId << buff.remainTime;
    }
    curSize = bufferWriter.WriteSize();

    header->size = bufferWriter.WriteSize();
    header->id = S_TEST;

    sendBuffer->Close(bufferWriter.WriteSize());

    return sendBuffer;
}
