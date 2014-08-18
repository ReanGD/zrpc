#include "SocketManager.h"

#include <utility>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include <zmq.h>

#include "ZException.h"


class zrpc::CSocketManager::impl
{
public:
    void *m_context = NULL;
    boost::mutex m_mutex;
};


zrpc::CSocketManager::CSocketManager(void)
    : pimpl(std::make_shared<impl>())
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    pimpl->m_context = zmq_ctx_new();
    if(pimpl->m_context == NULL)
		throw CZException();
}

zrpc::CSocketManager::~CSocketManager(void)
{
	try
	{
		Close();
	}catch(const CZException&)
	{
        //
	}
}

zrpc::CSocketManager::CSocketManager(CSocketManager &&rhs)
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    std::swap(pimpl, rhs.pimpl);
}

zrpc::CSocketManager &zrpc::CSocketManager::operator=(CSocketManager &&rhs)
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    std::swap(pimpl, rhs.pimpl);
	return *this;
}

std::shared_ptr<zrpc::CSocket> zrpc::CSocketManager::CreateServerSocket(const char* address, const tBinary& identity, bool is_sync)
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    if(pimpl->m_context == NULL)
        throw std::invalid_argument("Can not create a server socket, context is not created");

    auto socket_ptr = zmq_socket(pimpl->m_context, ZMQ_ROUTER);
	if (socket_ptr == NULL)
		throw CZException();

    std::shared_ptr<CSocket> socket(new CSocket(socket_ptr, is_sync));

    int rc = zmq_setsockopt(socket_ptr, ZMQ_IDENTITY, static_cast<const void*>(&identity[0]), identity.size());
    if (rc != 0)
        throw CZException();

    int router_mandatory = is_sync ? 1 : 0;
    rc = zmq_setsockopt (socket_ptr, ZMQ_ROUTER_MANDATORY, &router_mandatory, sizeof(router_mandatory));
    if (rc != 0)
        throw CZException();

    rc = zmq_bind(socket_ptr, address);
	if (rc != 0)
		throw CZException();    

	return socket;
}

std::shared_ptr<zrpc::CSocket> zrpc::CSocketManager::CreateClientSocket(const char* address, const tBinary& identity, bool is_sync)
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    if(pimpl->m_context == NULL)
        throw std::invalid_argument("Can not create a client socket, context is not created");

    auto socket_ptr = zmq_socket(pimpl->m_context, ZMQ_DEALER);
	if (socket_ptr == NULL)
		throw CZException();

    std::shared_ptr<CSocket> socket(new CSocket(socket_ptr, is_sync));

    int rc = zmq_setsockopt(socket_ptr, ZMQ_IDENTITY, static_cast<const void*>(&identity[0]), identity.size());
    if (rc != 0)
        throw CZException();

    int immediate = is_sync ? 1 : 0;
    rc = zmq_setsockopt(socket_ptr, ZMQ_IMMEDIATE, &immediate, sizeof(immediate));
    if (rc != 0)
        throw CZException();

    rc = zmq_connect(socket_ptr, address);
	if (rc != 0)
		throw CZException();

	return socket;
}

void zrpc::CSocketManager::Close(void)
{
    boost::lock_guard<boost::mutex> guard(pimpl->m_mutex);

    if(pimpl->m_context == NULL)
		return;
    int rc = zmq_ctx_destroy(pimpl->m_context);
	if(rc != 0)
		throw CZException();
    pimpl->m_context = NULL;
}
