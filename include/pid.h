#pragma once 

#include <thread>
#include <mutex>
#include <functional>
#include <unistd.h>
#include "gpio.h"

using pid_callback_t =  std::function<void(int)>;

struct PIDConstants
{
    float kP, kI, kD;
    int16_t max_output;
    uint16_t integral_max, integral_begin;
};

class PID
{
public:
    PID(const PIDConstants &pid_constants, const char* name);
    ~PID();

    void set_callback(pid_callback_t callback);
    void unset_callback();

    void set_target(int32_t target);

private:

    void pid_loop();
    
private:
    std::thread m_pid_thread;
    std::mutex m_callback_mutex;
    pid_callback_t m_callback;
    int32_t m_target;

    const PIDConstants m_pid_constants;
    const char* m_name;

    int16_t m_error, m_derivative;

    bool m_pid_running;

    static constexpr uint8_t  m_timeout_ms = 10;
    static constexpr uint16_t m_timeout_us = m_timeout_ms * 1000;
};