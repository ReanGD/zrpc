#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <functional>
#include <boost/thread/thread_only.hpp>

#include "Signals.h"

namespace helper
{

class CThreadPool
{
public:
    CThreadPool(void) = default;
    ~CThreadPool(void);
public:
    CThreadPool& Run(std::function<void (helper::CSignals& signal)> threadfunc)
    {
        m_threads.push_back(
            std::make_shared<boost::thread>(threadfunc, boost::ref(m_signal)));
        return *this;
    }
    CThreadPool& RunMulti(size_t cnt, std::function<void (helper::CSignals& signal)> threadfunc)
    {
        for(size_t i=0; i!=cnt; ++i)
            Run(threadfunc);
        return *this;
    }
private:
    helper::CSignals m_signal;
    std::list<std::shared_ptr<boost::thread>> m_threads;
};

}

#endif // THREADPOOL_H
