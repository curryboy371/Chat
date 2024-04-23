#include "ChattingClient.h"

ChatClient::ChatClient()
{

}

ChatClient::~ChatClient()
{

}

int ChatClient::ReceiveChat()
{
    char buf[MAX_SIZE] = { };
    string msg;

    while (true) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            //textColor(7, 0);
            msg = buf;
            //�г��� : �޽���
            std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ
            string user;
            ss >> user;
            if (user != my_nick) cout << buf << endl;
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

void ChatClient::SendMsg(const std::string& InMsg)
{
    const char* buffer = InMsg.c_str(); // string���� char* Ÿ������ ��ȯ
    send(client_sock, buffer, (int)strlen(buffer), 0);
}

void ChatClient::CreateSocket()
{
    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_sock == INVALID_SOCKET) {
        // ���� ó��: ���� ���� ����
        perror("Socket creation failed");
        return;
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);

    if (InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr) != 1) {
        // ���� ó��: IP �ּ� ��ȯ ����
        perror("IP address conversion failed");
        closesocket(client_sock);
        return;
    }
}


void ChatClient::CloseSocket()
{
    closesocket(client_sock);
}

int ChatClient::ConnnectSocket()
{
    CreateSocket();

    while (true)
    {
        cout << "Connecting..." << Retry_Count << endl;

        if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
            send(client_sock, my_nick.c_str(), (int)my_nick.length(), 0);
            return 0;
        }
        else
        {
            Retry_Count++;
        }

        if (Retry_Count >= MAX_TRY_COUNT)
        {
            cout << "Fail Connect!!!" << endl;
            return -1;
        }
    }

    return -2;
}

void ChatClient::DisConnnectSocket()
{
    CloseSocket();
}

