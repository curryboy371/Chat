#pragma once

/*
기존에 임시로
struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {  };
	int32 type = 0; // read, wirte, accpet, connect....

};

구조체로 사용하던 것을 아래와 같은 class event들로 나눠서 관리함




*/

class Session;

enum class EventType : uint8
{

	Connect,
	Disconnect,
	Accept,
	//PreRecv,
	Recv,
	Send,
};


// IocpEvnet ptr 와 OVERLAPPED ptr을 스위치하여 사용할 수 있게 상속
// 따라서 가상함수 사용하면 안되는 class임
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return eventype; }

public:
	EventType eventype;
	IocpObjectRef owner;
};


// TODO_CHAN Session을 멤버로 가지는 이유는?
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	//void SetSession(Session* session) { _session = session; }
	//Session* GetSession() { return _session; }

	SessionRef _session = nullptr;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}

private:
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}

private:
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}

private:
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	std::vector<SendBufferRef> sendBuffers;
private:
};