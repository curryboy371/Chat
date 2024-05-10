#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"

#include "Service.h"

Listener::~Listener()
{
    SocketUtils::Close(_socket);


    for (AcceptEvent* acceptEvent : _acceptEvens)
    {
        if (acceptEvent)
        {
            // TODO

            delete acceptEvent;
        }
    }

    _acceptEvens.clear();
}

bool Listener::StartAccept(ServerServiceRef service)
{
    _service = service;

    if (_service == nullptr)
    {
        return false;
    }

    _socket = SocketUtils::CreateSocket();

    if (_socket == INVALID_SOCKET)
    {
        return false;
    }

    if (_service->GetIocpCore()->Register(shared_from_this()) == false)
    {
        return false;
    }

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
    {
        return false;
    }

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
    {
        return false;
    }

    if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
    {
        return false;
    }

    if (SocketUtils::Listen(_socket) == false)
    {
        return false;
    }

    const int32 acceptCount = _service->GetMaxSessionCount();
    for (int32 i = 0; i < acceptCount; ++i)
    {
        AcceptEvent* acceptEvent = new AcceptEvent();
        acceptEvent->owner = shared_from_this();
        _acceptEvens.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }
    return false;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    SessionRef session = _service->CreateSession();
    acceptEvent->Init();
    acceptEvent->_session = session;

    DWORD byteReceived = 0;

    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0,
        sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
        OUT & byteReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
    {

        const int32 errCode = ::WSAGetLastError();
        if (errCode != WSA_IO_PENDING)
        {
            // 다시 Accept 시도
            RegisterAccept(acceptEvent);
        }
    }


}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    // acceptEvent을 사용하고 계속 재사용한다...
    // ok? 

    SessionRef session = acceptEvent->_session;

    if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 sizeOfSockAddr = sizeof(sockAddress);
    if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(NetAddress(sockAddress));

    cout << "Client Connected !~ " << endl;

    // todo


    // 이건 왜??
    RegisterAccept(acceptEvent);

}
