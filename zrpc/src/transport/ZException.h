#pragma once
#ifndef CZEXCEPTION_H
#define CZEXCEPTION_H

//#include <string>
#include <exception>


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

/*class CZTextException
     : public std::exception
{
public:
    CZTextException(const std::string& msg);
public:
    const char *what(void) const noexcept override;
private:
    std::string m_msg;
};*/

}

#endif // CZEXCEPTION_H
