#include "ZException.h"

#include <zmq.h>


zrpc::CZException::CZException(void)
	: m_error_code(zmq_errno())
{

}

zrpc::CZException::CZException(int error_code)
    : m_error_code(error_code)
{

}

const char *zrpc::CZException::what(void) const noexcept
{
	return zmq_strerror(m_error_code);
}

int zrpc::CZException::GetCode(void) const
{
	return m_error_code;
}

/*
CZTextException::CZTextException(const std::string &msg)
    : m_msg(msg)
{

}

const char *CZTextException::what(void) const noexcept
{
    return m_msg.c_str();
}
*/
