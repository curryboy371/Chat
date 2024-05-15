#include "pch.h"
#include "ChattingClient.h"

#include "Service.h"
#include "Session.h"

#include "ThreadManager.h"
CoreGlobal Core;

char sendData[500] = "sdfsdf";

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

        //SendBufferRef sendBuffer = std::make_shared<SendBuffer>(BUFFER_SIZE);
        //sendBuffer->CopyData(sendData, sizeof(sendData));

        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
        std::memcpy(sendBuffer->Buffer(), sendData, sizeof(sendData));
        sendBuffer->Close(sizeof(sendData));

        Send(sendBuffer);

    }
    virtual void OnDisconnected() override
    {
        cout << "ServerSession::OnDisconnected " << endl;

    }
    virtual int32 OnRecv(BYTE* buffer, int32 len)override
    {
        cout << "\nOnRecv Len = " << len;

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 1초 동안 대기


        //SendBufferRef sendBuffer = std::make_shared<SendBuffer>(BUFFER_SIZE);
        //sendBuffer->CopyData(sendData, sizeof(sendData));

        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
        std::memcpy(sendBuffer->Buffer(), sendData, sizeof(sendData));
        sendBuffer->Close(sizeof(sendData));


        Send(sendBuffer);
        return len;
    }

    virtual void OnSend(int32 len)override
    {
        cout << "\nOnSend Len = " << len;
    }



};

int main()
{
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 서버가 먼저 뜨게 1초 동안 대기
    
    NetAddress address = { L"127.0.0.1", 7777 };
    SessionFactory factory = []() {
        return std::make_shared<ServerSession>(); // 세션을 생성하여 반환
    };
    
    int32 SessionCount = MAX_CLIENT_SESSION;

    ClientServiceRef service = std::make_shared<ClientService>(address, std::make_shared<IocpCore>(), factory, SessionCount);
    ASSERT_CRASH(service->Start());
    
    for (int32 i = 0; i < 5; ++i)
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