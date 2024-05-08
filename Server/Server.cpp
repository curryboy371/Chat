#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"



#include "ThreadManager.h"

#include <iostream>
#include <thread>
CoreGlobal Core;


void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    std::cout << cause << " Error Code  : " << errCode << '\n';
}

const int32 BUFSIZE = 1000;


struct Session
{
    //WSAOVERLAPPED overlapped = {}; // OverlappedEx로 뺐음. 왜냐면?

    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;

};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNET,
};

struct OverlappedEx
{
    WSAOVERLAPPED overlapped = {  };
    int32 type = 0; // read, wirte, accpet, connect....

};



void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
    cout << "Data Recv Len Callback = " << recvLen << endl;
    
    // TODO
}


void WorkerThreadMain(HANDLE iocpHandle)
{
    while (true)
    {
        DWORD bytesTransferred = 0;
        Session* session = nullptr;
        OverlappedEx* overlappedEx = nullptr;

        // 완료된 일감이 있는지 체크
        BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

        if (ret = FALSE || bytesTransferred == 0)
        {
            // TODO 연결끊어짐
            continue;
        }


        ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

        cout << "Recv Data IOCP = " << bytesTransferred << endl;

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flags = 0;
        ::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
    }

}



int main()
{

    WSADATA wsa;

    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        return 0;
    }

    // 서버 리슨소켓 생성
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        return 0;
    }

    // 논브로킹 빼고..
    // 논블로킹 설정
    //u_long on = 1;
    //if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
    //{
    //    return 0;
    //}

    // 서버 허용 주서 설정
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    // 바인드 
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        return 0;
    }

    // 리슨
    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        return 0;
    }

    cout << "Accept" << '\n';

    // Overlapped Model ( callback)
    // 비동기 입출력함수가 완료되면 쓰레드마다 있는 APC Queue에 쌓임
    // Alertable Wait 상태로 들어가면서 APC Queue 비움
    // 단점 apc queue 쓰레드마다 존재, alertable wait도 부담스러움
    // 단점 이벤트 방식 소켓 : 이벤트 1:1 대응

    // IOCP Completion Port Model
    // APC > Completion Port ( 쓰레드마다 있는게 아닌, 한개의 APC Queue가 존재함
    // Alertable Wait > P 결과 처리를 GetQueuedCompletionStatus 
    // 쓰레드와 사용하기에 적합함


    std::vector<Session*> sessionManager;
    
    // CP 생성
    HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);


    // WorkerThreads
    for (int32 i = 0; i < 5; ++i)
    {
        GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });
    }

    // Main Thread는 Accept를 담당

    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            return 0;
        }

        Session* session = new Session();
        session->socket = clientSocket;
        sessionManager.push_back(session);

        cout << "Client Connected !" << '\n';


        // 소켓을 CP에 등록
        // key값은 구분자 아무거나...
        ::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/(ULONG_PTR)session, 0);

        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = BUFSIZE;

        

        OverlappedEx* overlappedEx = new OverlappedEx();
        overlappedEx->type = IO_TYPE::READ;


        DWORD recvLen = 0;
        DWORD flags = 0;

        ::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL); // pending이 거릴수도 바로 받을 수도..
    }


    GThreadManager->Join();
    //::closesocket(listenSocket);
    WSACleanup();

	return 0;
}

