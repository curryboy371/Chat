#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"

#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"

CoreGlobal Core;

int main()
{
    NetAddress address = { L"127.0.0.1", 7777 };
    SessionFactory factory = []() {
        return std::make_shared<GameSession>(); // 세션을 생성하여 반환
    };
    
    int32 SessionCount = 1;

    ServerServiceRef service = std::make_shared<ServerService>(address, std::make_shared<IocpCore>(), factory, SessionCount);
    ASSERT_CRASH(service->Start());

    for (int32 i = 0; i < 1; ++i)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    // 이렇게 하면 공용자원 안 건드나?
                    service->GetIocpCore()->Dispatch();
                }
            });
    }

    GThreadManager->Join();

	return 0;
}

