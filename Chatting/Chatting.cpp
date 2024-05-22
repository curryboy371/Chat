#include "pch.h"
#include "ChattingClient.h"

#include "Service.h"
#include "Session.h"

#include "ThreadManager.h"
#include "ClientPacketHandler.h"

CoreGlobal Core;

char sendData[DATA_SIZE] = "abcde";

class ServerSession : public PacketSession
{
public:

    ~ServerSession()
    {
        cout << "ServerSession::~ServerSession " << endl;
    }

    virtual void OnConnected() override
    {
        //cout << "ServerSession::OnConnected " << endl;
        Protocol::C_LOGIN pkt;
        auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        Send(sendBuffer);
    }
    virtual void OnDisconnected() override
    {
        cout << "ServerSession::OnDisconnected " << endl;

    }
    virtual void OnRecvPacket(BYTE* buffer, int32 len)override
    {
        PacketSessionRef session = GetPacketSessionRef();
        ClientPacketHandler::HandlePacket(session, buffer, len);
    }

    virtual void OnSend(int32 len)override
    {
        //cout << "\nOnSend Len = " << len;
    }

};

using CallbackType = std::function<void()>;

class Callback
{
public:
    Callback(CallbackType&& callback)
        :_callback(std::move(callback))
    {

    }

    void Excute()
    {
        _callback();
    }

private:
    CallbackType _callback;
};

int main()
{
    ClientPacketHandler::Init();


    //while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 서버가 먼저 뜨게 1초 동안 대기

        NetAddress address = { L"127.0.0.1", 7777 };
        SessionFactory factory = []() {
            return std::make_shared<ServerSession>(); // 세션을 생성하여 반환
        };

        int32 SessionCount = 1;

        ClientServiceRef service = std::make_shared<ClientService>(address, std::make_shared<IocpCore>(), factory, SessionCount);
        ASSERT_CRASH(service->Start());

        for (int32 i = 0; i < 1; ++i)
        {
            GThreadManager->Launch([=]()
                {
                    while (true)
                    {
                        service->GetIocpCore()->Dispatch();
                    }
                });
        }
    }


    GThreadManager->Join();
    
    return 0;
    



}