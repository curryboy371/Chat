#pragma once

#include "IocpCore.h"
#include "NetAddress.h"


class AcceptEvent;
class ServerService;
class Listener : public IocpObject
{

public:
	Listener() = default;
	~Listener();

public:

	// 외부 사용
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:

	// 인터페이스 구현 
	// IocpObject을(를) 통해 상속됨 
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;

	
private:
	//수신 관련...
	void RegisterAccept(AcceptEvent* acceptEvent); // 비동기 함수 걸어서
	void ProcessAccept(AcceptEvent* acceptEvent); // register 성공


protected:

	SOCKET _socket = INVALID_SOCKET;
	std::vector<AcceptEvent*> _acceptEvens;

	ServerServiceRef _service;
};

