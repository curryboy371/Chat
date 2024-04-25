#include "pch.h"

#include "ChattingServer.h"
#include "DB.h"


int main()
{
    WSADATA wsa;

    ChattingServer Server;
    
    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code)
    {
        Server.ServerInit();
        std::thread th1[MAX_CLIENT];
    
        for (int i = 0; i < MAX_CLIENT; ++i)
        {
            // 인원 수 만큼 thread ㅅ애성해서 각각의 클라이언트가 동시에 소통할 수 있도록..
            th1[i] = std::thread(&ChattingServer::AddClient, &Server);
        }
    
        while (true)
        {
            std::string text, msg;
    
            std::getline(cin, text);
            const char* buff = text.c_str();
            msg = Server.server_sock.user + " : " + buff;
            Server.ServerSendMessage(msg.c_str());
        }
    
    
        for (int i = 0; i < MAX_CLIENT; ++i)
        {
            th1[i].join();
        }
    
    
        Server.ServerRelease();
    }
    else
    {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }

    WSACleanup();

    //delete result;
    //delete pstmt;
    //delete con;
    //delete stmt;

	return 0;
}

