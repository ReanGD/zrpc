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

zrpc::timeout_error::timeout_error(const std::string& __arg)
    : std::runtime_error(__arg)
{

}

zrpc::timeout_error::~timeout_error(void)
{

}
