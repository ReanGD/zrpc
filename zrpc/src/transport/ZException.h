#pragma once
#ifndef CZEXCEPTION_H
#define CZEXCEPTION_H

#include <stdexcept>


namespace zrpc
{

class CZException
	 : public std::exception
{
public:
	CZException(void);
    CZException(int error_code);
public:
	const char *what(void) const noexcept override;
	int GetCode(void) const;
private:
	int m_error_code;
};

class timeout_error
        : public std::runtime_error
{
public:
  explicit timeout_error(const std::string& __arg);
  virtual ~timeout_error(void);
};

}

#endif // CZEXCEPTION_H
