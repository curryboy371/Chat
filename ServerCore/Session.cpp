#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

#include "Service.h"

Session::Session()
{
    _socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Disconnect(const WCHAR* cause)
{
    if (_connected.exchange(false) == false) 
    {
        // 기존에 이미 false이면 한번만 disconnect호출을 위해 return
        return;
    }

    std::wcout << "Disconenect " << cause << '\n';

    OnDisconnected(); 

    SocketUtils::Close(_socket);
    GetService()->ReleaseSession(GetSessionRef());

}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    // TODO

    switch (iocpEvent->eventype)
    {
    case EventType::Connect:
        ProcessConnect();
        break;

    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;

    case EventType::Send:
        ProcessSend(numOfBytes);
        break;


    default:
        break;
    }
}

void Session::RegisterConnect()
{
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
    {
        return;
    }

    //RecvEvent* recvEvent = new RecvEvent();
    _recvEvent.owner = shared_from_this(); // AddRef

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
    wsaBuf.len = len32(_recvBuffer);

    DWORD numOfBytes = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
    {
        int32 errCode = ::WSAGetLastError();

        if (errCode != WSA_IO_PENDING)
        {
            HandleError(errCode);
            _recvEvent.owner = nullptr; // Release REf
        }
    }
}

void Session::RegisterSend()
{
}

void Session::ProcessConnect()
{
    // connect 값 변경
    _connected.store(true); 

    // 세션 등록
    GetService()->AddSession(GetSessionRef());


    // 컨텐츠 기능 
    OnConnected();


    // 수신 등록
    RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes)
{
    _recvEvent.owner = nullptr; // release ref

    if (numOfBytes == 0)
    {
        Disconnect(L"Recv byte is 0");
        return;
    }

    //TODO
    std::cout << "Recv Data Len " << numOfBytes << '\n';


    // 수신등록
    // 이렇게 꼐속 recv 해줘야하는건가??
    RegisterRecv();

}

void Session::ProcessSend(int32 numOfBytes)
{
}

void Session::HandleError(int32 errCode)
{
    switch (errCode)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"Handle Error");
    default:
        // TODO Error Log..
        std::cout << "err..r.r." << '\n';
        break;
    }


}
