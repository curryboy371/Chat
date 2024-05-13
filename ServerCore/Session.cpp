#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"

#include "Service.h"
#include "IocpEvent.h"

Session::Session()
    :_recvBuffer(BUFFER_SIZE)
{
    _socket = SocketUtils::CreateSocket();
    if (_socket == UINT64_MAX)
    {
        int32 errCode = ::WSAGetLastError();
        cout << errCode << endl;
    }
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Send(BYTE* buffer, int32 len)
{
    // 버퍼 관리
    // SendEvnet를 어떤식으로 관리할지. 단일 or 복수
    // wsasend는 중첩하는지

    //temp
    SendEvent* sendEvent = new SendEvent;
    sendEvent->owner = shared_from_this(); // add ref
    sendEvent->buffer.resize(len);
    std::memcpy(sendEvent->buffer.data(), buffer, len);

    //WRITE_LOCK
    RegisterSend(sendEvent);
}

bool Session::Connect()
{
    return RegisterConnect();
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


    RegisterDisconnect();
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
    case EventType::Disconnect:
        ProcessDisconnect();
        break;

    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;

    case EventType::Send:
        ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
        break;


    default:
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected() == true)
    {
        return false;
    }

    // 커넥트는 클라이언트일때만 요청해야함
    if (GetService()->GetServiceType() != ServiceType::Client)
    {
        return false;
    }

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
    {
        int32 errCode = ::WSAGetLastError();
        cout << errCode << endl;
        return false;
    }

    if (SocketUtils::BindAnyAddress(_socket, 0/*남은 포트에서 아무거나 연동*/) == false)
    {
        int32 errCode = ::WSAGetLastError();
        cout << errCode << endl;
        return false;
    }

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this(); // Add Ref

    DWORD numOfBytes;
    SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
    sockAddr.sin_port;
    sockAddr.sin_addr;

    if(false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
    {
        int32 errCode = ::WSAGetLastError();
        if (errCode != WSA_IO_PENDING)
        {
            _connectEvent.owner = nullptr; // Release Ref
            return false;
        }
    }


    return true;

}

bool Session::RegisterDisconnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this(); // Add Ref

    DWORD nulOfBytes = 0;
    SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();

    if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
    {
        int32 errCode = ::WSAGetLastError();
        if (errCode != WSA_IO_PENDING)
        {
            _connectEvent.owner = nullptr; // Release Ref
            return false;
        }
    }

    return true;
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
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
    wsaBuf.len = _recvBuffer.FreeSize();

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

void Session::RegisterSend(SendEvent* sendEvent)
{
    if (IsConnected() == false)
    {
        return;
    }

    WSABUF wsaBuf;

    wsaBuf.buf = (char*)sendEvent->buffer.data();
    wsaBuf.len = (ULONG)sendEvent->buffer.size();


    DWORD numOfByte = 0;


    // WSASend는 Multi Thread에서 Safe한가?
    // 그렇지 않기 때문에 호출이전에 lock을 걸어줘야함.
    // 
    if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT & numOfByte, 0, sendEvent, nullptr))
    {
        int32 errCode = ::WSAGetLastError();
        if (errCode != WSA_IO_PENDING)
        {
            HandleError(errCode);
            sendEvent->owner = nullptr; // Release Ref
            delete sendEvent;
        }
    }



}

void Session::ProcessConnect()
{
    _connectEvent.owner = nullptr; // release ref 

    // connect 값 변경
    _connected.store(true); 

    // 세션 등록
    GetService()->AddSession(GetSessionRef());


    // 컨텐츠 기능 
    OnConnected();


    // 수신 등록
    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    _disconnectEvent.owner = nullptr;

}

void Session::ProcessRecv(int32 numOfBytes)
{
    _recvEvent.owner = nullptr; // release ref

    if (numOfBytes == 0)
    {
        Disconnect(L"Error Recv byte is 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect(L"Error OnWrite Overflow");
        return; 
    }

    int32 dataSize = _recvBuffer.DataSize();

    //컨텐츠 오버라이딩
    int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
    
    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        Disconnect(L"Error OnRead Overflow");
        return;
    }

    _recvBuffer.Clean();

    // 수신등록
    // 이렇게 꼐속 recv 해줘야하는건가??
    RegisterRecv();

}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
    sendEvent->owner = nullptr; // relese ref
    delete sendEvent;

    if (numOfBytes == 0)
    {
        Disconnect(L"Send Data is 0");
        return;
    }

    OnSend(numOfBytes);
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
