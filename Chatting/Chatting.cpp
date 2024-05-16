#include "pch.h"
#include "ChattingClient.h"

#include "Service.h"
#include "Session.h"

#include "ThreadManager.h"
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
        cout << "ServerSession::OnConnected " << endl;

        //SendBufferRef sendBuffer = std::make_shared<SendBuffer>(BUFFER_SIZE);
        //sendBuffer->CopyData(sendData, sizeof(sendData));
        PacketHeader header;
        header.id = 1;
        header.size = DATA_SIZE;

        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
        BufferWriter bufferWriter(sendBuffer->Buffer(), sendBuffer->AllocSize());
        bufferWriter << header;
        bufferWriter.Write(sendData, header.size);


        //check
        PacketHeader TemppacketHeader;
        BYTE tempByte[BUFFER_SIZE];

        std::memcpy(&TemppacketHeader, sendBuffer->Buffer(), sizeof(PacketHeader)); // heaer
        std::memcpy(&tempByte, sendBuffer->Buffer() + sizeof(PacketHeader), sizeof(sendData)); // data



        sendBuffer->Close(sendBuffer->AllocSize());

        Send(sendBuffer);

    }
    virtual void OnDisconnected() override
    {
        cout << "ServerSession::OnDisconnected " << endl;

    }
    virtual int32 OnRecvPacket(BYTE* buffer, int32 len)override
    {
        
        BYTE tempBuffer[BUFFER_SIZE];
        std::memcpy(tempBuffer, &buffer[4], len -4);
        //std::cout << "\nPacketSession::OnRecv : " << tempBuffer << packetSize + headerSize;

        BufferReader bufferReader(buffer, len);
        PacketHeader header;
        bufferReader >> header; // PacketHeader 크기만큼 pos를 이동, header data 넣어줌.

        //uint64 id;
        //uint32 hp;
        //bufferReader >> id >> hp; // 또 자료형 크기만큼 pos 이동... 이거 왜한거????

        char recvBuffer[BUFFER_SIZE];
        bufferReader.Read(recvBuffer, header.size); // pos가 이동된 상태... data만 읽으면 되지 않음?

        cout << "\nRecvPacket Len = " << len << " Data " << recvBuffer;

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 1초 동안 대기

        SendBufferRef sendBuffer = GSendBufferManager->Open(BUFFER_SIZE);
        BufferWriter bufferWriter(sendBuffer->Buffer(), sendBuffer->AllocSize());
        bufferWriter << header;
        bufferWriter.Write(recvBuffer, header.size);
        sendBuffer->Close(sendBuffer->AllocSize());

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
    
    for (int32 i = 0; i < 8; ++i)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    service->GetIocpCore()->Dispatch();
                }
            });
    }

    GThreadManager->Join();
    
    return 0;
    



}