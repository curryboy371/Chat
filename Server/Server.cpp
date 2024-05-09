#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"



#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Listener.h"
CoreGlobal Core;


int main()
{
    Listener* temp = new Listener;

    ListenerRef listener = std::make_shared<Listener>(*temp);

    listener->StartAccept(NetAddress(L"127.0.0.1", 7777));

    for (int32 i = 0; i < 5; ++i)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    // 이렇게 하면 공용자원 안 건드나?
                    GlobalCore.Dispatch();
                }
            });
    }

    GThreadManager->Join();

	return 0;
}

