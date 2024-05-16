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



    BufferReader bufferReader(buffer, len);
    PacketHeader header;
    bufferReader >> header; // PacketHeader 크기만큼 pos를 이동, header data 넣어줌.

    BYTE recvBuffer[BUFFER_SIZE];
    bufferReader.Read(recvBuffer, header.size); // pos가 이동된 상태... data만 읽으면 되지 않음?

    int32 headerSize = sizeof(PacketHeader);

    bool breflect = false;

    if (breflect)
    {
        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
        BufferWriter bufferWriter(sendBuffer->Buffer(), sendBuffer->AllocSize());
        bufferWriter << header;
        bufferWriter.Write(recvBuffer, header.size);
        sendBuffer->Close(sendBuffer->AllocSize());

        //for (int32 i = 0; i < 10; ++i)
        {
            GSessionManager.Broadcast(sendBuffer);
        }
    }

    return header.size;
}
