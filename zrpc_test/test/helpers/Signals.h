#pragma once
#ifndef SIGNALS_H
#define SIGNALS_H

#include <atomic>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace helper
{

class CSignals
{
public:
    CSignals(void);
    ~CSignals(void) = default;
public:
    void Send(const uint32_t number);
    void Wait(const uint32_t number);
private:
    boost::mutex m_mutex;
    std::atomic<uint32_t> m_cur_number;
    boost::condition_variable m_cv;
};

}

#endif // SIGNALS_H
