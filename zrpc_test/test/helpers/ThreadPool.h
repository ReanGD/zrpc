#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <boost/thread/thread_only.hpp>

namespace helper
{

class CThreadPool
{
public:
    CThreadPool(void) = default;
    ~CThreadPool(void);
public:
    template<typename F>
    CThreadPool& Run(F threadfunc)
    {
        m_threads.push_back(std::make_shared<boost::thread>(threadfunc));
        return *this;
    }
private:
    std::list<std::shared_ptr<boost::thread>> m_threads;
};

}

#endif // THREADPOOL_H
