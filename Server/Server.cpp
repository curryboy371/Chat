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
    int32 sendBytes = 0;

};




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

    //select model
    // 소켓 함수 호출이 성공할지 안할지를 미리 알 수 있음.
    // how?

    // 문제상황
    // 수신 버퍼에 데이터가 없는데 read를 함.
    // 송신 버퍼가 꽈 찼는데 write를 함
    // 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
    // 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방
    
    // socket set
    // 1 읽기[2] 쓰기[ ] 예외[ ] 관찰 대상 등록
    // OutOfBand는 send 마지막 인자 MSG_OOB로 보내는 특별한 데이터
    // 받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음.
    // 2 select (readSet, writeSet, exceptSet) 관찰 시작
    // 적어도 하나의 소켓이 준비되면 리턴 > 낙오자는 알아서 제거
    // 남는 소켓 체크해서 진행...


    // FD_ZERO :  비움
    // FD_SET : 소켓을 넣음
    // FD_CLR : 소켓을 제거
    // FD_ISSET : 소켓이 Set에 들어있으면 0아닌 값을 리턴

    vector<Session> sessions;
    sessions.reserve(100);

    fd_set reads;
    fd_set writes;

    while (true)
    {
        // 매 루프마다 초기화 하고 select 가 실행됨.
        // select가 호출되고 나면 관찰이 필요한 소켓만 선택되어 나머지는 제거됨
        // 소켓 셋 초기화
        FD_ZERO(&reads);
        FD_ZERO(&writes);


        // listenSocket 등록
        FD_SET(listenSocket, &reads);


        // 소켓 등록
        for (Session& s : sessions)
        {
            // 최초에는 read 등록
            if (s.recvBytes <= s.sendBytes)
            {
                FD_SET(s.socket, &reads);
            }
            else
            {
                FD_SET(s.socket, &writes);
            }
        }

        // Option 마지막 timeOut 인자 설정 가능함
        int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
        if (retVal == SOCKET_ERROR)
        {
            break;
        }

        // Listener 소켓 체크
        if (FD_ISSET(listenSocket, &reads))
        {
            SOCKADDR_IN clientAddr;
            int32 addrLen = sizeof(clientAddr);
            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                sessions.push_back(Session{ clientSocket });
                cout << "Client Connected " << endl;

            }
        }

        // 나머지 소켓 체크
        for (Session& s : sessions)
        {
            // Read
            if (FD_ISSET(s.socket, &reads))
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen <= 0)
                {
                    // TODO : sessions remove
                    continue;
                }

                s.recvBytes = recvLen;
            }

            // Write
            if (FD_ISSET(s.socket, &writes))
            {
                // 블로킹 모드 > 모든 데이터 다 보냄
                // 논블로킹 모드 - 일부만 보낼 수 있음 ( 상대 수신 버퍼 상황에 따라..)
                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR)
                {
                    //todo : session remove
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }
            }
        }
    }





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

