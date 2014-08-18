#pragma once
#ifndef CSOCKETGROUP_H
#define CSOCKETGROUP_H

#include <list>
#include <memory>
#include <chrono>


namespace zrpc
{

class CSocket;

class CSocketGroup
{
public:
    CSocketGroup(void);
    CSocketGroup(std::shared_ptr<CSocket> socket);
    ~CSocketGroup(void) = default;
    CSocketGroup(CSocketGroup &&) = delete;
    CSocketGroup(const CSocketGroup&) = delete;
    CSocketGroup &operator=(CSocketGroup &&) = delete;
    void operator=(const CSocketGroup&) = delete;
public:
    void Add(std::shared_ptr<CSocket> socket);
    void Remove(std::shared_ptr<CSocket> socket);
    std::list<std::shared_ptr<CSocket>> Recv(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
private:
    class impl; std::shared_ptr<impl> pimpl;
};

}

#endif // CSOCKETGROUP_H
