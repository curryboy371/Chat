#include "pch.h"
#include "Service.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
    :_type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{
}

Service::~Service()
{
}

bool Service::Start()
{
    return false;
}

void Service::CloseService()
{
    // TODO
}

SessionRef Service::CreateSession()
{
    SessionRef session = _sessionFactory();
    Session* sessionptr = session.get();

    IocpObject* iocpptr = (IocpObject*)session.get();
    
    if (_iocpCore->Register(std::shared_ptr<IocpObject>(iocpptr)) == false)
    {
        return nullptr;
    }

    return session;
}

void Service::AddSession(SessionRef session)
{
    // WRITE_LOCK

    _sessionCount++;
    _sessions.insert(session);

}

void Service::ReleaseSession(SessionRef session)
{
    // WRITE_LOCK
    ASSERT_CRASH(_sessions.erase(session) != 0);

    --_sessionCount;
}

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
    :Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool ClientService::Start()
{


    // TODO
    return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
    :Service(ServiceType::Server, address, core, factory, maxSessionCount)
{

}

bool ServerService::Start()
{
    if (CanStart() == false)
    {
        return false;
    }

    _listener = std::make_shared<Listener>();
    if (_listener == nullptr)
    {
        return false;
    }

    ServerServiceRef service = std::static_pointer_cast<ServerService>(shared_from_this());

    if (_listener->StartAccept(service) == false)
    {
        return false;
    }

    return true;
}

void ServerService::CloseService()
{
    Service::CloseService();
}
