#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"

#include "ServerPacketHandler.h"

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

            // send open, writer init size, static data fill...
            PKT_S_TEST_WRITE pkt(1, 2, 3);

            // dynamic data reserve, offset, buffcount set 
            PKT_S_TEST_WRITE::BuffsList buffList =  pkt.ReserveBuffsList(3);
            buffList[0] = { 1, 1.f };
            buffList[1] = { 2, 2.f };
            buffList[2] = { 3, 3.f };


            PKT_S_TEST_WRITE::VictimList victimList1 = pkt.ReserveVictimList(&buffList[0], 3);
            victimList1[0].userId = 1;
            victimList1[0].remainHp = 100;
            victimList1[1].userId = 2;
            victimList1[1].remainHp = 200;
            victimList1[2].userId = 3;
            victimList1[2].remainHp = 300;


            PKT_S_TEST_WRITE::VictimList victimList2 = pkt.ReserveVictimList(&buffList[1], 3);
            victimList2[0].userId = 1;
            victimList2[0].remainHp = 100;
            victimList2[1].userId = 2;
            victimList2[1].remainHp = 200;
            victimList2[2].userId = 3;
            victimList2[2].remainHp = 300;
            
            PKT_S_TEST_WRITE::VictimList victimList3 = pkt.ReserveVictimList(&buffList[2], 2);
            victimList3[0].userId = 1;
            victimList3[0].remainHp = 100;
            victimList3[1].userId = 2;
            victimList3[1].remainHp = 200;

            SendBufferRef sendBuffer = pkt.CloseAndReturn();

            GSessionManager.Broadcast(sendBuffer);
        }

 
    }


    GThreadManager->Join();

	return 0;
}

