#include "SocketGroup.h"

#include <boost/thread/thread.hpp>
#include <boost/chrono/duration.hpp>

#include <zmq.h>

#include "Socket.h"
#include "ZException.h"


class zrpc::CSocketGroup::impl
{
public:
    impl() {}
    ~impl() {}
public:
    void Recalc(void);
public:
    std::vector<zmq_pollitem_t> m_items;
    std::list<std::shared_ptr<CSocket>> m_sockets;
};

void zrpc::CSocketGroup::impl::Recalc(void)
{
    m_items.resize(m_sockets.size());

    size_t i = 0;
    for(const auto it: m_sockets)
    {
        zmq_pollitem_t item = { it->Native(), 0, ZMQ_POLLIN, 0 };
        m_items[i++] = item;
    }
}


zrpc::CSocketGroup::CSocketGroup(void)
    : pimpl(std::make_shared<impl>())
{

}

zrpc::CSocketGroup::CSocketGroup(std::shared_ptr<CSocket> socket)
    : pimpl(std::make_shared<impl>())
{
    Add(socket);
}

void zrpc::CSocketGroup::Add(std::shared_ptr<CSocket> socket)
{
    pimpl->m_sockets.push_back(socket);
    pimpl->Recalc();
}

void zrpc::CSocketGroup::Remove(std::shared_ptr<CSocket> socket)
{
    pimpl->m_sockets.remove(socket);
    pimpl->Recalc();
}

std::list<std::shared_ptr<zrpc::CSocket>> zrpc::CSocketGroup::Recv(std::chrono::milliseconds timeout/*= std::chrono::milliseconds::max()*/)
{
    std::list<std::shared_ptr<CSocket>> result;
    if(pimpl->m_sockets.empty())
    {
        boost::chrono::milliseconds boost_timeout(timeout.count());
        boost::this_thread::sleep_for(boost_timeout);
        return result;
    }

    int rc = zmq_poll(&pimpl->m_items[0], static_cast<int>(pimpl->m_items.size()), static_cast<long>(timeout.count()));
    if(rc < 0)
        throw CZException();
    if(rc == 0)
        return result;

    auto socket_it = pimpl->m_sockets.cbegin();
    for(auto it: pimpl->m_items)
    {
        if(it.revents & ZMQ_POLLIN)
            result.push_back(*socket_it);
        ++socket_it;
    }

    return result;
}
