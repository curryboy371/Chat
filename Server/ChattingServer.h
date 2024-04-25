#pragma once

#include "DB.h"


struct SOCKET_INFO
{
    SOCKET sck = 0;
    string user = "";
    bool bIsConnect = false;
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

    void DeleteClient(const boost::uuids::uuid& Inuid);

    void ServerSendMessage(const char* msg);

    void ClientSendMessage(const char* msg, const boost::uuids::uuid& Inuid);


#pragma region Receive

    void Recv(const boost::uuids::uuid& Inuid);
    void RecvCreateAccount(const boost::uuids::uuid& Inuid, const json& InjsonData);
    void RecvLogin(const boost::uuids::uuid& Inuid, const json& InjsonData);
    void RecvMsg(const boost::uuids::uuid& Inuid, const json& InjsonData);


#pragma endregion Receive

#pragma region DB
    void DBInit();
    void CreateAccount(const boost::uuids::uuid& Inuid, const std::string& name, const std::string& passwd);

#pragma endregion DB



public:
    std::map<boost::uuids::uuid, SOCKET_INFO> sck_Map;
    SOCKET_INFO server_sock;
    int client_count = 0;


private:
    std::mutex mutex; // Mutex for synchronizing access to sck_Map

    DB myDB;

};