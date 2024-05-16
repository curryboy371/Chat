#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "TimeCheckManager.h"

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

int32 GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
    //cout << "\nOnRecv Len = " << len;

    //SendBufferRef sendBuffer = std::make_shared<SendBuffer>(BUFFER_SIZE); // 이러면 보낼때마다 생성하는데?
    //sendBuffer->CopyData(buffer, len);

    int32 headerSize = sizeof(PacketHeader);
    PacketHeader packetHeader;
    BYTE dataBuffer[DATA_SIZE];

    std::memcpy(&packetHeader, buffer, headerSize); // heaer
    std::memcpy(&dataBuffer, buffer + headerSize, packetHeader.size); // data

    bool breflect = false;

    if (breflect)
    {
        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);

        std::memcpy(sendBuffer->Buffer(), reinterpret_cast<BYTE*>(&packetHeader), headerSize); // heaer
        std::memcpy(sendBuffer->Buffer() + headerSize, &buffer[headerSize], packetHeader.size);   // data

        sendBuffer->Close(sizeof(PacketHeader) + packetHeader.size);

        //for (int32 i = 0; i < 10; ++i)
        {
            GSessionManager.Broadcast(sendBuffer);
        }
    }

    return packetHeader.size;
}
