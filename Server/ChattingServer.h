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
    std::vector<SOCKET_INFO> sck_list; // ����� Ŭ���̾�Ʈ ���ϵ��� ������ �迭 ����.
    SOCKET_INFO server_sock; // ���� ���Ͽ� ���� ������ ������ ���� ����.
    int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
};

