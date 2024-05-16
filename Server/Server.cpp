#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"

char sendData[DATA_SIZE] = "server_broad";

CoreGlobal Core;

int main()
{
    NetAddress address = { L"127.0.0.1", 7777 };
    SessionFactory factory = []() {
        return std::make_shared<GameSession>(); // 세션을 생성하여 반환
    };
    
    int32 SessionCount = 5;

    ServerServiceRef service = std::make_shared<ServerService>(address, std::make_shared<IocpCore>(), factory, SessionCount);
    ASSERT_CRASH(service->Start());

    for (int32 i = 0; i < 5; ++i)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    service->GetIocpCore()->Dispatch();
                }
            });
    }


    // main thread에서만... 
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 1초 동안 대기

        if (GSessionManager.GetSessionCount() > 0)
        {
            PacketHeader packetHeader;
            packetHeader.id = 1;
            packetHeader.size = DATA_SIZE;

            SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);

            std::memcpy(sendBuffer->Buffer(), reinterpret_cast<BYTE*>(&packetHeader), sizeof(PacketHeader)); // heaer
            std::memcpy(sendBuffer->Buffer() + sizeof(PacketHeader), sendData, sizeof(sendData));   // data

            PacketHeader TemppacketHeader;
            BYTE tempByte[BUFFER_SIZE];

            std::memcpy(&TemppacketHeader, sendBuffer->Buffer(), sizeof(PacketHeader)); // heaer
            std::memcpy(&tempByte, sendBuffer->Buffer() + sizeof(PacketHeader), sizeof(sendData)); // data

            sendBuffer->Close(sizeof(PacketHeader) + sizeof(sendData));
            GSessionManager.Broadcast(sendBuffer);
        }

 
    }


    GThreadManager->Join();

	return 0;
}

