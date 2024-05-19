#include "pch.h"
#include "ServerPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

/* 컨텐츠 수동 작업 */
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    std::cout << "\nHandle_INVALID:: id " << header->id << endl;
    return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
    GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);

    // validation

    Protocol::S_LOGIN loginPkt;
    loginPkt.set_success(true);

    // player정보 받아오고...

    // ingame id 발급
    static Atomic<uint64> idGenerator = 1;
    {
        auto player = loginPkt.add_players();
        player->set_name(u8"dbname...");
        player->set_playertype(Protocol::PLAYER_TYPE_ARCHER);
        
        PlayerRef playerRef = std::make_shared<Player>();
        playerRef->playerId = idGenerator++;
        playerRef->name = player->name();
        playerRef->type = player->playertype();
        playerRef->ownerSession = gameSession;

        gameSession->_players.push_back(playerRef);

    }

    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(loginPkt);
    session->Send(sendBuffer);

    return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
    GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);

    uint64 index = pkt.playerindex();

    // validation

    PlayerRef player = gameSession->_players[index];
    GRoom.Enter(player);

    Protocol::S_ENTER_GAME enterGamePkt;
    enterGamePkt.set_success(true);
    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
    player->ownerSession->Send(sendBuffer);
    return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
    std::cout << pkt.msg() << endl;

    Protocol::S_CHAT chatPkt;
    chatPkt.set_msg(u8"broadcst : " + pkt.msg());
    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);

    GRoom.Broadcast(sendBuffer);

    return true;
}
