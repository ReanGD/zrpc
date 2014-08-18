#include "Signals.h"


helper::CSignals::CSignals()
    : m_cur_number(UINT32_MAX)
{

}

void helper::CSignals::Send(const uint32_t number)
{
    boost::unique_lock<boost::mutex> locker(m_mutex);
    m_cur_number = number;
    m_cv.notify_all();
}

void helper::CSignals::Wait(const uint32_t number)
{
    boost::unique_lock<boost::mutex> locker(m_mutex);
    while (m_cur_number.load() != number)
        m_cv.wait(locker);
}
