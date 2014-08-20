#pragma once
#ifndef CSOCKET_H
#define CSOCKET_H

#include <list>
#include <vector>
#include <chrono>
#include <stdint.h>

namespace zrpc
{

using tBinary = std::vector<uint8_t>;
using tBinaryPackage = std::list<tBinary>;

class CSocket
{
    friend class CSocketGroup;
    friend class CSocketManager;
private:
    CSocket(void) = delete;
    CSocket(void* socket, bool is_sync);
public:	
    ~CSocket(void);
    CSocket(CSocket &&) = delete;
	CSocket(const CSocket&) = delete;
    CSocket &operator=(CSocket &&) = delete;
	void operator=(const CSocket&) = delete;
private:
    void* Native(void);
public:
    bool Send(const tBinaryPackage& package);
    tBinaryPackage Recv(void);
    bool Recv(tBinaryPackage& package, const std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
	void Close(void);
private:
    void* m_socket = nullptr;
    bool m_is_sync;
};

}

#endif // CSOCKET_H
