#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "TimeCheckManager.h"

int32 GameSession::OnRecv(BYTE* buffer, int32 len)
{
    cout << "\nOnRecv Len = " << len;

    SendBufferRef sendBuffer = std::make_shared<SendBuffer>(4096); // 이러면 보낼때마다 생성하는데?
    sendBuffer->CopyData(buffer, len);

    //for (int32 i = 0; i < 10; ++i)
    {
        GSessionManager.Broadcast(sendBuffer);
    }
    return len;
}

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
    cout << "\nOnSend Len = " << len;
}
