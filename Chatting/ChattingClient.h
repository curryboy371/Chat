#pragma once
#include "pch.h"


class ChatClient
{
public:
	ChatClient();
	virtual ~ChatClient();

	int ConnnectSocket();
	void DisConnnectSocket();

	int ReceiveChat();
	void SendMsg(const std::string& InMsg);

private:
	void CreateSocket();
	void CloseSocket();

private:
	SOCKET client_sock;
	SOCKADDR_IN client_addr = {};
	string my_nick;

	int Retry_Count = 0;
	const int MAX_TRY_COUNT = 10;
};
