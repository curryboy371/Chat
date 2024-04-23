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

    server_addr.sin_port = htons(7777); // 127.0.0.1:7777 //127 �Ƴ�?

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // Any�� ���� ȣ��Ʈ�� 127.0.0.1�� ��Ƶ� �ǰ� localhost�� ��Ƶ� �ǰ� ���� �� ����ϰ� �� �� �ִ�. �װ��� INADDR_ANY�̴�.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // ������ ���� ������ ���Ͽ� ���ε��Ѵ�.

    listen(server_sock.sck, SOMAXCONN); // ������ ��� ���·� ��ٸ���.

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

    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

    recv(new_client.sck, buf, MAX_SIZE, 0); // Winsock2�� recv �Լ�. client �� ���� nickname�� ����.

    new_client.user = string(buf);

    string msg = ">" + new_client.user + " enter the room";

    cout << msg << '\n';
    sck_list.push_back(new_client);

    //print_clients();

    // ����!!
    std::thread th(&ChattingServer::RecvMsg, this, client_count);
    
    th.detach(); //?
    client_count++;
    cout << "������ ������ �� : " << client_count << "��" << endl;
    ServerSendMessage(msg.c_str());
}

void ChattingServer::DeleteClient(int index)
{
    std::thread th(&ChattingServer::AddClient, this);
    closesocket(sck_list[index].sck);
    client_count--;
    cout << "������ ������ �� : " << client_count << "��" << endl;
    sck_list.erase(sck_list.begin() + index);
    th.join();
}

void ChattingServer::ServerSendMessage(const char* msg)
{
    // broadcast 

    for (int i = 0; i < client_count; ++i)
    {
        send(sck_list[i].sck, msg, MAX_SIZE, 0); // ��� client���� ����
    }
}

void ChattingServer::ClientSendMessage(const char* msg, const int sender_idx)
{
    for (int i = 0; i < client_count; ++i)
    {
        if (i != sender_idx)
        {
            send(sck_list[i].sck, msg, MAX_SIZE, 0); // ���� ������ ��� client���� ����
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
    std::string msg = "�ӼӸ� [" + sck_list[index].user + "] : " + dm;

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
            // ���� �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ��.
            // 0���� ũ�ٴ� ���� msg�� �� ��.

            std::string whisper(buff);
            //int position = whisper.find(" ", 0);
            //int message = position - 0;
            //std::string flag = whisper.substr(0, message);
            if (string(buff) == "/����")
            {
                msg = ">" + sck_list[index].user + "����";
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
