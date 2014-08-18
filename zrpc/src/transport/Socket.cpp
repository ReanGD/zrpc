#include "Socket.h"

#include <stdexcept>
#include <utility>
#include <zmq.h>

#include "ZException.h"


zrpc::CSocket::CSocket(void* socket, bool is_sync)
	: m_socket(socket)
    , m_is_sync(is_sync)
{

}

zrpc::CSocket::~CSocket()
{
	try
	{
		Close();
	}catch(const CZException&)
	{
        //
	}
}

void *zrpc::CSocket::Native(void)
{
    return m_socket;
}

bool zrpc::CSocket::Send(const tBinaryPackage& package)
{
    auto it = package.begin();
    if(it == package.end())
        throw std::length_error("Sending a package without data");

    bool is_end(false);
    while(!is_end)
    {
        auto bin_data = *it;
        is_end = (++it == package.end());

        int flags = (is_end ? 0 : ZMQ_SNDMORE) | (m_is_sync ? 0 : ZMQ_DONTWAIT);
        int rc = zmq_send(m_socket, &bin_data[0], bin_data.size(), flags);
        if(rc == -1)
        {
            int error_code = zmq_errno();
            if(m_is_sync && ((error_code == EAGAIN) || (error_code == EHOSTUNREACH)))
                return false;

            throw CZException(error_code);
        }

        if(static_cast<size_t>(rc) != bin_data.size())
            throw std::length_error("Sending a package incomplete");
    }

    return true;
}

class AutoCloseMsg
{
public:
    AutoCloseMsg(zmq_msg_t* msg)
        : m_msg(msg)
    {

    }
    ~AutoCloseMsg(void)
    {
        zmq_msg_close(m_msg);
    }
private:
    zmq_msg_t* m_msg;
};

void zrpc::CSocket::Recv(tBinaryPackage& package)
{
    zmq_msg_t msg;

    package.clear();
    do
    {
        if(zmq_msg_init(&msg) != 0)
            throw CZException();

        AutoCloseMsg msg_close(&msg);
        int recv_size = zmq_msg_recv(&msg, m_socket, 0);
        if(recv_size < 0)
            throw CZException();

        const uint8_t* begin = static_cast<uint8_t*>(zmq_msg_data(&msg));
        const uint8_t* end = begin + static_cast<size_t>(recv_size);
        package.push_back(tBinary(begin, end));

    }while(zmq_msg_more(&msg));
}

void zrpc::CSocket::Close(void)
{
    if(!m_socket)
		return;

    int linger = 0;
    int rc = zmq_setsockopt(m_socket, ZMQ_LINGER, &linger, sizeof(linger));
    if (rc != 0)
        throw CZException();

    rc = zmq_close(m_socket);
	if(rc != 0)
		throw CZException();

    m_socket = nullptr;
}