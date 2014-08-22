#include "ThreadPool.h"

#include <utility>

helper::CThreadPool::~CThreadPool(void)
{
    for(auto& it: m_threads)
        if(it.second && it.first->joinable())
            it.first->join();

    m_signal.Send(FINISH);

    for(auto& it: m_threads)
        if((!it.second) && it.first->joinable())
            it.first->join();
}

helper::CThreadPool &helper::CThreadPool::Run(tThreadFunc threadfunc, bool is_finish /*= false*/)
{
    m_threads.push_back(
                std::make_pair(std::make_shared<boost::thread>(threadfunc, boost::ref(m_signal)), is_finish));
    return *this;
}

helper::CThreadPool &helper::CThreadPool::RunAndFinish(tThreadFunc threadfunc)
{
    Run(threadfunc, true);
    return *this;
}

helper::CThreadPool &helper::CThreadPool::RunMulti(size_t cnt, tThreadFunc threadfunc)
{
    for(size_t i=0; i!=cnt; ++i)
        Run(threadfunc);
    return *this;
}
