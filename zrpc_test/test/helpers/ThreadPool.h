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
    using tThreadFunc = std::function<void (helper::CSignals& signal)>;
public:
    const static uint32_t FINISH = UINT32_MAX;
public:
    CThreadPool(void) = default;
    ~CThreadPool(void);
public:
    CThreadPool& Run(tThreadFunc threadfunc, bool is_finish = false);
    CThreadPool& RunAndFinish(tThreadFunc threadfunc);
    CThreadPool& RunMulti(size_t cnt, tThreadFunc threadfunc);
    CThreadPool& RunMultiAndFinish(size_t cnt, tThreadFunc threadfunc);
private:
    helper::CSignals m_signal;
    std::list<std::pair<std::shared_ptr<boost::thread>,bool>> m_threads;
};

}

#endif // THREADPOOL_H
