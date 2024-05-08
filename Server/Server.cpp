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
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[BUFSIZE] = {};
    int32 recvBytes = 0;

    WSAOVERLAPPED overlapped = {};
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
    cout << "Data Recv Len Callback = " << recvLen << endl;
    
    // TODO
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

    // 논블로킹 설정
    u_long on = 1;
    if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
    {
        return 0;
    }

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

    // Overlapped IO ( 비동기 + 논블로킹)
    // Overlapped 함수 걸기 ( WSARecv, WSASend)
    // Overlapped 함수 성공 여부 확인
        // 성공 : 결과 처리
        // 실패 : 사유 ㅊ케ㅡ


    // WSASend, WSARecv 
    // 1 비동기 입출력 소켓
    // 2 WSABUF 배열의 시작 주소 + 개수 // Scatter-Gather
    // 3 Send / Recv Byte 
    // 4 상세 옵션 0
    // 5 WSAOVERLAPPED 구조체 주소
    // 6 입출력 완료되면 OS가 호출할 CallbackFunc

    // Overlapped Model - Callback
    // 비도기 입출력 지원하는 소켓 생성, 통지 받는 이벤트 객체 생성
    // 비동기 입출력 함수 호출 ( 완료 루틴 시작 주소 넘겨줌)
    // 비동기 작업이 바로 완료되지 않으면 WSA_IO_PENDING 오류
    // // 비동기입추력 함수 호출한 쓰레드를 Alertable Wait 상태로 만듬
    // ex waitforsingleobjectgex, wailtformultipleobjectex, sleepex, wsawaitformultipleevnets
    // 비동기 IO 완료되면, 운영체제는 완료 루틴 호출
    // 완료 루틴 호출 모두 끝나면, 스레드는 alertable wait 상태에서 빠져나옴



    // void CompletionRoutine()
    // param1 오류 발생시 0아닌 값
    // param2 전송 바이트 수
    // param3 비동기 입출력 함수 호출시 넘겨준 WSAVOVERLAPPED 구조체 주소
    // param4 0
    // Overlapped Model Event Send & Recv에만 적용해봄

    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket;
        while (true)
        {
            clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                break;
            }

            if (::WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }

            // Error 
            return 0;
        }


        Session session = Session{ clientSocket };
        WSAEVENT wasEvent = ::WSACreateEvent();
        //session.overlapped.hEvent = wasEvent;

        cout << "Client Connected !" << endl;
        
        int32 test = 0;
        while (true)
        {
            WSABUF wasBuf;
            wasBuf.buf = session.recvBuffer;
            wasBuf.len = BUFSIZE;


            DWORD recvLen = 0;
            DWORD flags = 0;

            if (::WSARecv(clientSocket, &wasBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
            {
                if (::WSAGetLastError() == WSA_IO_PENDING)
                {
                    // Pending
                    //::WSAWaitForMultipleEvents(1, &wasEvent, TRUE, WSA_INFINITE, FALSE); // 음.. 이렇게 하면 블로킹 아냐?? 다른 로직 실행 가능한가
                    ++test;
                    ::SleepEx(INFINITE, TRUE);
                    // Alertable Wait 
                    //::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
                }
                else
                {
                    // TODO Error
                    break;
                }
            }

            //cout << "Data Recv Len = " << recvLen << endl;

        }
    }


    ::closesocket(listenSocket);
    WSACleanup();








    //WSADATA wsa;
    //ChattingServer Server;
    
    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    //int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    //if (!code)
    //{
    //    Server.ServerInit();
    //    std::thread th1[MAX_CLIENT];
    //
    //    for (int i = 0; i < MAX_CLIENT; ++i)
    //    {
    //        // 인원 수 만큼 thread ㅅ애성해서 각각의 클라이언트가 동시에 소통할 수 있도록..
    //        th1[i] = std::thread(&ChattingServer::AddClient, &Server);
    //    }
    //
    //    while (true)
    //    {
    //        std::string text, msg;
    //
    //        std::getline(cin, text);
    //        const char* buff = text.c_str();
    //        msg = Server.server_sock.user + " : " + buff;
    //        Server.ServerSendMessage(msg.c_str());
    //    }
    //
    //
    //    for (int i = 0; i < MAX_CLIENT; ++i)
    //    {
    //        th1[i].join();
    //    }
    //
    //
    //    Server.ServerRelease();
    //}
    //else
    //{
    //    cout << "프로그램 종료. (Error code : " << code << ")";
    //}
    //WSACleanup();

    //delete result;
    //delete pstmt;
    //delete con;
    //delete stmt;

	return 0;
}

