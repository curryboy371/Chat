#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "TimeCheckManager.h"
#include "ServerPacketHandler.h"

void GameSession::OnConnected()
{
    GameSessionRef session = std::static_pointer_cast<GameSession>(shared_from_this());
    GSessionManager.Add(session);

    if (GSessionManager.GetSessionCount() == MAX_CLIENT_SESSION)
    {
        GTimeCheckManager.EndTime();
        GTimeCheckManager.PrintTime();
    }
    else
    {
        if (GSessionManager.GetSessionCount() == 1)
        {
            GTimeCheckManager.StartTime();
        }

        std::cout << "Session Count : " << GSessionManager.GetSessionCount() << endl;
    }
}

void GameSession::OnDisconnected()
{
    GameSessionRef session = std::static_pointer_cast<GameSession>(shared_from_this());
    GSessionManager.Remove(session);
}

void GameSession::OnSend(int32 len)
{
    //cout << "\nOnSend Len = " << len;
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
    //cout << "\nOnRecv Len = " << len;
    PacketSessionRef session = GetPacketSessionRef();
    ServerPacketHandler::HandlePacket(session, buffer, len);
}
