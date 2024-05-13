#include "pch.h"
#include "ChattingClient.h"

#include "Service.h"
#include "Session.h"

#include "ThreadManager.h"
CoreGlobal Core;

char sendBuffer[] = "Hello World";

class ServerSession : public Session
{
public:

    ~ServerSession()
    {
        cout << "ServerSession::~ServerSession " << endl;
    }

    virtual void OnConnected() override
    {
        cout << "ServerSession::OnConnected " << endl;

        Send((BYTE*)sendBuffer, sizeof(sendBuffer));

    }
    virtual void OnDisconnected() override
    {
        cout << "ServerSession::OnDisconnected " << endl;

    }
    virtual int32 OnRecv(BYTE* buffer, int32 len)override
    {
        cout << "OnRecv Len = " << len << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 동안 대기

        Send((BYTE*)sendBuffer, sizeof(sendBuffer));
        return len;
    }

    virtual void OnSend(int32 len)override
    {
        cout << "OnSend Len = " << len << endl;
    }



};

int main()
{
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 서버가 먼저 뜨게 1초 동안 대기
    
    NetAddress address = { L"127.0.0.1", 7777 };
    SessionFactory factory = []() {
        return std::make_shared<ServerSession>(); // 세션을 생성하여 반환
    };
    
    ClientServiceRef service = std::make_shared<ClientService>(address, std::make_shared<IocpCore>(), factory, 1);
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