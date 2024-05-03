#include "pch.h"
#include "ChattingClient.h"

void RunChatSession(ChatClient& client)
{
    if (client.ConnnectSocket() != 0)
    {
        return;
    }

    std::thread th(&ChatClient::ReceiveChat, &client); // 에러 반환을 위해 먼저 받을 수 있도록...

    client.Main();

    string text;
    while (getline(cin, text)) {
        if (text == "/종료") {
            client.SendMsg(text);
            client.DisConnnectSocket();
            break;
        }
        client.SendMsg(text);
    }
    th.join();
}

int main()
{
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 동안 대기

    WSADATA wsa;
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
    {
        return 1;
    }

    // 클라이언트 소켓 생성
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        return 1;
    }

    // 논블로킹 설정
    u_long on = 1;
    if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
    {
        return 0;
    }

    // 서버 주소 설정
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = ::htons(7777);

    // Connect
    while (true)
    {
        if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            // 블록
            if (::WSAGetLastError() == WSAEWOULDBLOCK)
            {
                /*
                만약 connect 함수가 블록되어 있는 상태에서 오류가 발생한다면, 
                WSAEWOULDBLOCK 에러 코드가 반환될 수 있습니다. 
                이는 비동기 소켓(non-blocking socket)에서 소켓 연결이 아직 완료되지 않았음을 나타내는 에러 코드입니다.
                따라서 if 문에서 WSAEWOULDBLOCK 에러를 검사하고 있는데, 
                이 경우에는 블록되지 않고 다음 루프로 넘어가게 됩니다. 
                이는 연결이 아직 완료되지 않았지만, 계속해서 연결을 시도하겠다는 것을 나타냅니다.
                
                */
                continue;
            }

            // 이미 연결된 상태에서 에러가 난다면 중지
            if (::WSAGetLastError() == WSAEISCONN)
            {
                break;
            }

            break;
        }
    }

    cout << "Connected to Server" << endl;

    char sendBuffer[100] = "Hi Server";
    WSAEVENT wasEvent = ::WSACreateEvent();
    WSAOVERLAPPED overlapped = {};
    overlapped.hEvent = wasEvent;

    //send
    while (true)
    {
        WSABUF wasBuf;
        wasBuf.buf = sendBuffer;
        wasBuf.len = 100;


        DWORD sendLen = 0;
        DWORD flags = 0;

        if (::WSASend(clientSocket, &wasBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
        {
            if (::WSAGetLastError() == WSA_IO_PENDING)
            {
                // Pending
                ::WSAWaitForMultipleEvents(1, &wasEvent, TRUE, WSA_INFINITE, FALSE);
                ::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
            }
            else
            {
                // TODO Error
                break;
            }
        }

        cout << "Data Send Len = " << sendLen << endl;
        std::this_thread::sleep_for(std::chrono::seconds(30)); // 1초 동안 대기
    }

    //ChatClient client;
    //RunChatSession(client);

    // 소켓 리소스 반환
    ::closesocket(clientSocket);

    // 윈속 종료
    ::WSACleanup();
    return 0;
}