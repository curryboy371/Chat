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
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "WSAStartup failed." << endl;
        return 1;
    }

    ChatClient client;
    RunChatSession(client);


    WSACleanup();
    return 0;
}