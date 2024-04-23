#include "pch.h"
#include "ChattingClient.h"

void RunChatSession(ChatClient& client)
{
    if (client.ConnnectSocket() != 0)
    {
        return;
    }

    std::thread th(&ChatClient::ReceiveChat, &client);
    string text;
    while (getline(cin, text)) {
        if (text == "/종료") {
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