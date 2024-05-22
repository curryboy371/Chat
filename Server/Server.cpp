#include "pch.h"

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
#include "JobTimer.h"

CoreGlobal Core;

enum
{
    WORKER_TICK = 64
};

// 모든 스레드에서 job queue를 골고루 처리하도록...

void DoWorkerJob(ServerServiceRef& service)
{
    while (true)
    {
        LEndTickCount = ::GetTickCount64() + WORKER_TICK; // 여기도 타임아웃을 설정해둔다고 보면 됨.

        // 네트워크 입출력 처리 > 인게임 로직 처리 
        service->GetIocpCore()->Dispatch(10); // evevnt가 걸리지 않으면 대기하지 않도록 timeout 걸어줌

        // 예약된 일감 처리
        ThreadManager::DistributeReservedJobs();

        //타임아웃에 걸리면 글로벌 큐에 넣음
        ThreadManager::DoGlobalQueueWork();
    }
}

int main()
{


    Protocol::S_CHAT chatPkt;
    chatPkt.set_msg(u8"broadcst : ");
    auto sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
    GRoom->DoAsync(&Room::Broadcast, sendBuffer);

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
        GThreadManager->Launch([&service]()
            {
                DoWorkerJob(service);
            });
    }

    //main thread
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        // DO Timer
        GRoom->DoTimer(10, [] { cout << "Hello 10" << endl; });
        GRoom->DoTimer(20, [] { cout << "Hello 20" << endl; });
        GRoom->DoTimer(30, [] { cout << "Hello 30" << endl; });
    }
    //DoWorkerJob(service);


    GThreadManager->Join();

	return 0;
}

