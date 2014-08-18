#include "ThreadPool.h"


helper::CThreadPool::~CThreadPool(void)
{
    for(auto& it: m_threads)
        if(it->joinable())
            it->join();
}
