#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore()
{
	// iocp handle 반환
	// 여기서 하는 것과 클라 소켓 등록할때랑 정확한 차이점 파악해야함.
	// iocphandle을 반환받으려는 목적으로 보이긴 함.
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef IocpObject)
{
	// 클라이언트 소켓 등록.
	// 세션, 즉 소켓을 어떻게 관리할 것인가...	

	//return ::CreateIoCompletionPort(IocpObject->GetHandle(), _iocpHandle, /*key*/ reinterpret_cast<ULONG_PTR>(IocpObject), 0);
	return ::CreateIoCompletionPort(IocpObject->GetHandle(), _iocpHandle, /*key*/ 0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD  numOfBytes = 0;
	ULONG_PTR key = 0;
	//IocpObject* IocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef IocpObject = iocpEvent->owner;
		IocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();

		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO 
			IocpObjectRef IocpObject = iocpEvent->owner;
			IocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}
