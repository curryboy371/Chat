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

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    if (pkt.success() == false)
    {
        return false;
    }

    if (pkt.players().size() == 0)
    {
        // character craete
    }

    Protocol::C_ENTER_GAME enterGamePkt;
    enterGamePkt.set_playerindex(0); 
    auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
    session->Send(sendBuffer);

    return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
    return false;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
    std::cout << pkt.msg() << endl;
    return true;
}
