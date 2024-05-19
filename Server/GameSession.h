#pragma once

#include "Session.h"

class GameSession : public PacketSession
{
public:
    ~GameSession()
    {
        cout << "GameSession::~GameSession " << endl;
    }
    virtual void OnConnected() override;
    virtual void OnDisconnected() override;
    virtual void OnSend(int32 len) override;
    //virtual int32 OnRecv(BYTE* buffer, int32 len) override;
    virtual void OnRecvPacket(BYTE* buffer, int32 len);


public:
    std::vector<PlayerRef> _players;

};
