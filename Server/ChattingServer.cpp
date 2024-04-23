#include "pch.h"
#include "ChattingServer.h"

ChattingServer::ChattingServer()
{
}

ChattingServer::~ChattingServer()
{
}

void ChattingServer::ServerInit()
{
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


    SOCKADDR_IN server_addr = {};

    server_addr.sin_family = AF_INET; // internet type socket

    server_addr.sin_port = htons(7777); // 127.0.0.1:7777 //127 아냐?

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있다. 그것이 INADDR_ANY이다.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.

    listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.

    server_sock.user = "server";

}

void ChattingServer::ServerRelease()
{
    closesocket(server_sock.sck);

}

void ChattingServer::AddClient()
{
    SOCKADDR_IN addr = {  };
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = {};

    ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

    recv(new_client.sck, buf, MAX_SIZE, 0); // Winsock2의 recv 함수. client 가 보낸 nickname을 받음.

    new_client.user = string(buf);

    string msg = ">" + new_client.user + " enter the room";

    cout << msg << '\n';
    sck_list.push_back(new_client);

    //print_clients();

    // 여기!!
    std::thread th(&ChattingServer::RecvMsg, this, client_count);
    
    th.detach(); //?
    client_count++;
    cout << "▷현재 접속자 수 : " << client_count << "명" << endl;
    ServerSendMessage(msg.c_str());
}

void ChattingServer::DeleteClient(int index)
{
    std::thread th(&ChattingServer::AddClient, this);
    closesocket(sck_list[index].sck);
    client_count--;
    cout << "▷현재 접속자 수 : " << client_count << "명" << endl;
    sck_list.erase(sck_list.begin() + index);
    th.join();
}

void ChattingServer::ServerSendMessage(const char* msg)
{
    // broadcast 

    for (int i = 0; i < client_count; ++i)
    {
        send(sck_list[i].sck, msg, MAX_SIZE, 0); // 모든 client에게 전송
    }
}

void ChattingServer::ClientSendMessage(const char* msg, const int sender_idx)
{
    for (int i = 0; i < client_count; ++i)
    {
        if (i != sender_idx)
        {
            send(sck_list[i].sck, msg, MAX_SIZE, 0); // 나를 제외한 모든 client에게 전송
        }
    }
}

void ChattingServer::SendWhisper(int position, std::string sbuff, int index)
{
    int cur_position = position + 1;
    position = sbuff.find(" ", cur_position);
    int len = position - cur_position;
    std::string receiver = sbuff.substr(cur_position, len);
    cur_position = position + 1;
    std::string dm = sbuff.substr(cur_position);
    std::string msg = "귓속말 [" + sck_list[index].user + "] : " + dm;

    for (int i = 0; i < client_count; ++i)
    {
        if (receiver.compare(sck_list[i].user) == 0)
        {
            send(sck_list[i].sck, msg.c_str(), MAX_SIZE, 0);
        }
    }
}

void ChattingServer::RecvMsg(int index)
{
    char buff[MAX_SIZE] = {};
    std::string msg = "";

    while (true)
    {
        ZeroMemory(&buff, MAX_SIZE);

        if (recv(sck_list[index].sck, buff, MAX_SIZE, 0) > 0)
        {
            // 오류 발생하지 않으면 recv는 수신된 바이트 수를 반환함.
            // 0보다 크다는 것은 msg가 온 것.

            std::string whisper(buff);
            //int position = whisper.find(" ", 0);
            //int message = position - 0;
            //std::string flag = whisper.substr(0, message);
            if (string(buff) == "/종료")
            {
                msg = ">" + sck_list[index].user + "퇴장";
                cout << msg << endl;
                ServerSendMessage(msg.c_str());
                DeleteClient(index);
                return;
            }
            else
            {
                cout << whisper << endl;
                ClientSendMessage(whisper.c_str(), index);
            }

        }
    }
}
