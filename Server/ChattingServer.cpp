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

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(7777); 

    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_sock.sck, SOMAXCONN); 

    server_sock.user = "server";

    DBInit();
}

void ChattingServer::ServerRelease()
{
    closesocket(server_sock.sck);

}

void ChattingServer::AddClient()
{
    SOCKADDR_IN addr = {  };
    int addrsize = sizeof(addr);
    char buff[MAX_SIZE] = {};

    memset(&addr, 0, addrsize);

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    boost::uuids::uuid new_uuid = boost::uuids::random_generator()();

    {
        std::lock_guard<std::mutex> guard(mutex);

        sck_Map.insert(std::make_pair(new_uuid, new_client)); 
    }

    std::thread th(&ChattingServer::Recv, this, new_uuid);
    th.detach(); //?
}

void ChattingServer::DeleteClient(const boost::uuids::uuid& Inuid)
{
    //std::thread th(&ChattingServer::AddClient, this);
    closesocket(sck_Map[Inuid].sck);

    {
        std::lock_guard<std::mutex> guard(mutex);
        sck_Map.erase(Inuid);
        client_count--;
    }

    cout << "DeleteClient : " << client_count << "ggg" << endl;
    //th.join();
}

void ChattingServer::ServerSendMessage(const char* msg)
{
    // broadcast 
    for (const auto& sck_pair : sck_Map)
    {
        if (sck_pair.second.bIsConnect)
        {
            send(sck_pair.second.sck, msg, MAX_SIZE, 0);
        }
    }
}

void ChattingServer::ClientSendMessage(const char* msg, const boost::uuids::uuid& Inuid)
{
    for (const auto& sck_pair: sck_Map)
    {
        if (sck_pair.second.bIsConnect)
        {
            if (sck_pair.first != Inuid)
            {
                send(sck_Map[Inuid].sck, msg, MAX_SIZE, 0);
            }
        }

    }
}

void ChattingServer::Recv(const boost::uuids::uuid& Inuid)
{
    char buff[MAX_SIZE] = {};

    while (true)
    {
        memset(&buff, 0, MAX_SIZE);

        if (recv(sck_Map[Inuid].sck, buff, MAX_SIZE, 0) > 0)
        {
            json receivedJson = json::parse(buff);

            if (receivedJson["type"] == "CreateAccount")
            {
                RecvCreateAccount(Inuid, receivedJson);
            }
            else if (receivedJson["type"] == "Login")
            {
                RecvLogin(Inuid, receivedJson);
            }
            else if (receivedJson["type"] == "Message")
            {
                RecvMsg(Inuid, receivedJson);
            }
            else if (receivedJson["type"] == "Exit")
            {
                break;
            }
        }
    }
}

void ChattingServer::RecvCreateAccount(const boost::uuids::uuid& Inuid, const json& InjsonData)
{
    string username = InjsonData["content"]["username"];
    string password = InjsonData["content"]["password"];

    string msg;
    if (username.empty() || password.empty())
    {
        msg = "invaild account info!!!";
        send(sck_Map[Inuid].sck, msg.c_str(), MAX_SIZE, 0);
        return;
    }

    CreateAccount(Inuid, username, password);
}

void ChattingServer::RecvLogin(const boost::uuids::uuid& Inuid, const json& InjsonData)
{
    // 검증해주고.
    string username = InjsonData["content"]["username"];
    string password = InjsonData["content"]["password"];

    int result = myDB.UserLoginCheck(username, password);

    if (result == 0)
    {

        send(sck_Map[Inuid].sck, "loginsuccess", MAX_SIZE, 0);


        sck_Map[Inuid].bIsConnect = true;
        string msg = ">" + username + " enter the room";
        cout << msg << '\n';
        client_count++;
        cout << "AddClient : " << client_count << "" << endl;
        ServerSendMessage(msg.c_str());
    }
    else
    {
        string errmsg;
        if (result == 10000)
        {
            errmsg = "invaild account info";
        }
        else
        {
            errmsg = "unknown error e " + result;
        }
        send(sck_Map[Inuid].sck, errmsg.c_str(), MAX_SIZE, 0);
    }

}

void ChattingServer::RecvMsg(const boost::uuids::uuid& Inuid, const json& InjsonData)
{
    string msg = InjsonData["content"]["message"];

    if (msg == "/Exit")
    {
        msg = ">" + sck_Map[Inuid].user + "Exit";
        cout << msg << endl;
        ServerSendMessage(msg.c_str());
        DeleteClient(Inuid);
        return;
    }
    else
    {
        cout << msg << endl;
        ClientSendMessage(msg.c_str(), Inuid);
    }
}

void ChattingServer::DBInit()
{
    myDB.DBConnect();

    myDB.CreateTable();
}

void ChattingServer::CreateAccount(const boost::uuids::uuid& Inuid, const std::string& name, const std::string& passwd)
{
    int result = myDB.UserTableAdd(name, passwd);
    
    string msg;

    if (result == 0)
    {
        msg = "create account success " + name;
    }
    if (result != 0)
    {
        switch (result)
        {
        case 1062:
            msg = "duplicated name";
            break;
        default:
            msg = "unknown error";
            break;
        }
    }
    send(sck_Map[Inuid].sck, msg.c_str(), MAX_SIZE, 0);
}