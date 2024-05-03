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

   
    // WSAEventSelect = (WSAEventSelect 함수가 핵심)
    // 소켓과 관련된 네트워크 이벤트를 이벤트 객체를 통해 감지함


    // 이벤트 객체 관련 함수
    // 생성 : WSACreateEvent ( 수동 리셋 Manual-Reset + Non-Signaled State)
    // 삭제 : WSACloseEvent
    // 상태 감지 : WSAWaitForMultipleEvents
    // 네트워트 이벤트 알아내는 함수 : WSAEnumNetworkEvents

    // 소켓 - 이벤트 객체를 1:1로 연도하여 사용
    // WSAEventSelect(socket, event, networkEvents); - 소켓과 이벤트를 1:1 연동
    // 관찰 가능한 네트워크 이벤트 옵션들.. 
    // FD_ACCEPT : 접속한 클라가 있을때 accept
    // FD_READ : 데이터 수신 가능 recv, recvfrom
    // FD_WRITE : 데이터 송신 가능 send, snedto
    // FD_CLOSE : 상대가 접속 종료
    // FD_CONNECT : 통신을 위한 절차 완료 connect
    // FD_OOB 


    // 주의 
    // WSAEventSelect 함수를 호출하면, 해당 소켓은 자동으로 Non-Blocking Mode로 전환됨
    // Accept() 함수가 리턴하는 소켓은 listen Socket과 동일한 속성을 가짐
    // 따라서 closeSocket은 FD_Read, Fd_write 등을 다시 등록해야함
    // 드물게 WSAWOULDBLOCK 오류가 뜰 수 있으므로 예외처리 필요함
    // *** 이벤트 발생시 적절한 소켓 함수 호출해야함, 그렇지 못하면 다음번에 동일 네트워크 이벤트가 발생 안함 (두번다시 발생하지 않음 )


    // 이벤트 객체를 한번 만들어두고 select 처럼 모두 리셋하지는 않음.


    // 등록은 WSAEventSelect, 통지는 WSAWaitForMultipleEvents 함수에서 받음.

    // WSAWaitForMultipleEvents 
    // param 1 count, event
    // param 2 wailt all 모두 기ㅏ릴지, 하나만 완료되어도 ok할지
    // param3 timeout 
    // param4 false
    // return d완료된 첫번째 인덱스 

    // WSAEnumNetworkEvents
    // param1 socket
    // param2 eventObj ( 소켓과 연동된 이벤트 객체) > 이벤트 객체를 non-signaled로 변경해줌
    // param3 networkevent : 네트워크 이벤트 | 오류 정보 반환



    vector<WSAEVENT> wsaEvents; // 소켓과 1:1로 대칭할 이벤트
    vector<Session> sessions; // 이벤트와 1:1로 대칭할 소켓

    // 이벤트 생성
    WSAEVENT listenEvent = ::WSACreateEvent();

    // 소켓-이벤트 1:1 맵핑 비슷하게 pushback() 
    wsaEvents.push_back(listenEvent);
    sessions.push_back(Session{ listenSocket });


    // listenSocket 소켓 NonBlocking Mode로 전환
    // 1대1 관계이므로 listensocket - lisetevent객체를 넣어줌
    // Accept, Close 이벤트를 캐치를 expect
    if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
    {
        // ERROR 
        return 0;
    }


    // client 소켓이 n개가 되는 경우 하나씩 체크하는게 아니라
    // n개를 모두 등록하고 event 발생하는 것만 처리하도록 함.


    while (true)
    {
        bool fWaitAll = false;
        // 이벤트의 개수 n 개
        // 이벤트의 주소 
        // 하나라도 완료되면 빠져나오도록... ( 다 기다리지 않도록
        // 타임아웃 없음
        // return 인덱스 

        // ??? 2번째 이벤트의 주소는 현재 listen이벤트의 주소 하나만을 넣어주고 있는데
        // 이것은 배열의 첫번째 주소를 넘겨주면 나머지는 다 등록되는 것인가? 아니면 listenEvent만 등록하는 상황이라 그런것인가..
        // fWailtAll 옵션이 있어서 소켓 하나만 처리하는게 아니긴 한데...
        // = 등록된 모든 이벤트가 완료될때까지 기다리고 이후 한번에 하나씩 처리 한다는 방식으로 보여짐!
        int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], fWaitAll, WSA_INFINITE, FALSE);
        // 가장먼저 signaled 되는 이벤트의 인덱스가 나옴. 이 이벤트는 소켓과 인덱스가 동일하게 연결해뒀으므로 소켓에 접근할 수 있음.

        if (index == WSA_WAIT_FAILED)
        {
            // ERROR
            continue;
        }

        // 인덱스 조정
        index -= WSA_WAIT_EVENT_0;

        // WSAEnumNetworkEvents에서 eventReset기능 포함하여 실행 안해도됨
        //::WSAResetEvent(wsaEvents[index]);

        // 어떤 이벤트에서 걸렸는지 체크
        WSANETWORKEVENTS networkEvents;



        // 이벤트를 다시 non signaled 상태로 전환함 ( signal 정보를 아래에서 체크하여 수행할 것이므로 ) 
        // param1 소켓을 꺼냄
        // param2 해당하는 이벤트 꺼냄
        // param3 관찰하려는 이벤트 정보 or 에러 
        if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
        {
            // Error
            continue;
        }

        // Listener 소켓 체크
        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            // Error Check
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                continue;
            }

            SOCKADDR_IN clientAddr;
            int32 addrLen = sizeof(clientAddr);

            // 클라이언트 소켓을 받고
            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << endl;

                // 연결되면 받은 클라이언트 소켓도 1:1 매칭 시키게 이에 대응하는 이벤트 생성
                WSAEVENT clientEvent = WSACreateEvent();

                // 클라이언트 소켓 맵핑
                // 소켓-이벤트 1:1 맵핑 비슷하게 pushback() 
                wsaEvents.push_back(clientEvent);
                sessions.push_back(Session{ clientSocket });
                if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) // 리스너가 아니므로 옵션 교체
                {
                    // ERROR 
                    return 0;
                }


            }
        }

        // Client Session 소켓 체크
        if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
        {
            // Error Check
            if (networkEvents.lNetworkEvents & FD_READ && networkEvents.iErrorCode[FD_READ_BIT] != 0)
            {
                continue;
            }

            // Error Check
            if (networkEvents.lNetworkEvents & FD_WRITE && networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
            {
                continue;
            }

             
            // event-socket의 인덱스가 연결되었으므로 동일 인덱스로 session에 접근
            Session& s = sessions[index];

            // Read
            if (s.recvBytes == 0)
            {
                int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);

                if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) // woudboock는 예외이므로 비포함
                {
                    // ERROR
                    continue;
                }

                s.recvBytes = recvLen;
                cout << "Recv Data = " << recvLen << endl;
            }


            // Write
            if (s.recvBytes > s.sendBytes)
            {
                int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
                {
                    // Error
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes)
                {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }

                cout << "Send Data " << sendLen << endl;
            }


            // FD_CLOSE 처리
            if (networkEvents.lNetworkEvents & FD_CLOSE)
            {
                // Remove 
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

