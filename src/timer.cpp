#include "timer.h"
#include <iostream>
#include <string>
#include "log.h"

Timer::Timer() :
    m_interrupt_timer{false}
{
}


Timer::~Timer()
{
    stop_timer();
    Log::log_info("Timer::~Timer - Timer terminated gracefully...");
}


void Timer::start_timer(const uint32_t timeout_ms)
{
    // Terminate thread first if already running
    stop_timer();

    {
        std::lock_guard<std::mutex> lck(m_mutex);
        m_interrupt_timer.store(false);
    }
    m_handler_thread = std::thread(&Timer::run_timer, this, timeout_ms);
}


void Timer::stop_timer()
{
    {
        std::lock_guard<std::mutex> lck(m_mutex);
        m_interrupt_timer.store(true);
    }
    m_timer_condition.notify_all();

    if (m_handler_thread.joinable()) {
        m_handler_thread.join();
    }
}


void Timer::set_timer_callback(timeout_callback_t callback)
{
    std::lock_guard<std::mutex> lck(m_timeout_callback_mutex);
    m_timeout_callback = callback;
}

void Timer::unset_timer_callback()
{
    std::lock_guard<std::mutex> lck(m_timeout_callback_mutex);
    m_timeout_callback = nullptr;
}


void Timer::run_timer(const uint32_t timeout_ms)
{
    //! lock mutex => check predicate => IF predicate == false => wait ELSE continue program
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_timer_condition.wait_for(lock,
                                    std::chrono::milliseconds(timeout_ms),
                                    [this]{return (m_interrupt_timer.load());})) {
        // Timer got interrupted
        Log::log_info("Timer::run_timer - timer interrupted, terminating thread");
        return;
    }

    // Timeout: notify subscriber
    std::lock_guard<std::mutex> lck(m_timeout_callback_mutex);
    if (m_timeout_callback) { m_timeout_callback(); }

    Log::log_info("Timer::run_timer - Terminating thread...");
}
