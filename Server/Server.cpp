﻿#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"

#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

#include "Room.h"

char sendData[DATA_SIZE] = "server_broad";

CoreGlobal Core;


void HealByValue(int64 target, int32 value)
{
    cout << "HealByValue " << target << " " << value << " " << endl;
}

int main()
{

    //GRoom.PushJob(&Room::Broadcast, sendBuffer);

    GRoom.PushJob<void, int64, int32>(HealByValue, 4, 4);


    ServerPacketHandler::Init();

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
            GRoom.FlushJob();
        }
    }
    GThreadManager->Join();

	return 0;
}

