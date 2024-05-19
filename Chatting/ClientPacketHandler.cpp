#include "pch.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


/* 컨텐츠 수동 작업 */
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    std::cout << "\nHandle_INVALID:: id " << header->id << endl;
    return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
    std::cout << "\nHandle_S_TEST:: id hp " << pkt.id() << " " << pkt.hp() << endl;

    return true;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    return true;
}
