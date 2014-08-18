#pragma once
#ifndef CSOCKETMANAGER_H
#define CSOCKETMANAGER_H

#include <memory>
#include "Socket.h"

namespace zrpc
{

class CSocketManager
{
public:
	CSocketManager(void);
	~CSocketManager(void);
	CSocketManager(CSocketManager &&rhs);
	CSocketManager(const CSocketManager&) = delete;
	CSocketManager &operator=(CSocketManager &&rhs);
	void operator=(const CSocketManager&) = delete;
public:
    std::shared_ptr<CSocket> CreateServerSocket(const char* address, const tBinary& identity, bool is_sync);
    std::shared_ptr<CSocket> CreateClientSocket(const char* address, const tBinary& identity, bool is_sync);
public:
	void Close(void);
private:
    class impl; std::shared_ptr<impl> pimpl;
};

}

#endif // CSOCKETMANAGER_H
