#pragma once


#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();

public:
	// 외부에서 사용
	void Send(BYTE* buffer, int32 len);
	bool Connect();
	void Disconnect(const WCHAR* cause); // 세션 연결 끊기



	std::shared_ptr<Service> GetService() { return _service.lock(); } // lock()함수는 weak를 shared로 변환함
	void SetService(std::shared_ptr<Service> service) { _service = service; }



public:
	// 정보 관련
	void SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress GetNetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }

	bool IsConnected() { return _connected; }
	SessionRef GetSessionRef() { return std::static_pointer_cast<Session>(shared_from_this()); }

public:
	// 인터페이스 
	// IocpObject을(를) 통해 상속됨
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
	/* 전송 관련*/
	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend(SendEvent* sendEvent);

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(SendEvent* sendEvent, int32 numOfBytes);

	void HandleError(int32 errCode);

protected:
	// 컨텐츠에서 오버라이딩 해서 사용할 함수
	virtual void OnConnected() {}
	virtual int32 OnRecv(BYTE* buffer, int32 len) { return 0; }
	virtual void  OnSend(int32 Len) {}
	virtual void OnDisconnected() {}


public:
	// temp
	BYTE _recvBuffer[1000];

private:
	std::weak_ptr<Service> _service;

	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _connected = false;


private:
	// USE_LOCK;

	// 수신기능


	// 송신 기능
	
private:
	// IocpEvent 재사용을 위함
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;

	RecvEvent _recvEvent;

};

