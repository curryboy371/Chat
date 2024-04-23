#pragma once

struct SOCKET_INFO
{
    SOCKET sck = 0;
    string user = "";
};

class ChattingServer
{
public:
    ChattingServer();
    ~ChattingServer();

public:
    void ServerInit();
    void ServerRelease();

    void AddClient();

    void DeleteClient(int index);

    void ServerSendMessage(const char* msg);

    void ClientSendMessage(const char* msg, const int sender_idx);

    void SendWhisper(int position, std::string sbuff, int index);

    void RecvMsg(int index);

public:
    std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
    SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.
    int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
};

