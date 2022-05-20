#pragma once 

#include <thread>
#include <mutex>
#include <functional>
#include <unistd.h>
#include "gpio.h"
#include "config.h"


/*****
 * 
 * (LOOP START)
 *   Get Sensor Value
 *   Calculate PID 
 *   Send Output
 * (LOOP END)
 * 
 ******/

using pid_callback_t   = std::function<void(int)>; // void move_motor(int pwm_value);
using pid_sensor_get_t = std::function<int(void)>; // int get_encoder(); 

class PID
{
public:
    PID(const PID_Base &pid_base);
    ~PID();

    void set_callback(pid_callback_t callback);
    void unset_callback();

    void set_sensor_get(pid_sensor_get_t sensor_get);
    void unset_sensor_get();

    void set_target(int32_t target);

private:

    void pid_loop();
    
private:
    std::thread m_pid_thread;    // pid loop thread
    std::mutex m_callback_mutex; // Callback mutex protection
    std::mutex m_sensor_mutex;   // Sensor get mutex protection

    pid_callback_t m_callback; // Callback function
    pid_sensor_get_t m_sensor_get; // Sensor get function

    int32_t m_target;

    const PID_Base m_pid_base;

    int16_t m_error, m_derivative;

    bool m_pid_running;

    static constexpr uint8_t  m_timeout_ms = 10;
    static constexpr uint16_t m_timeout_us = m_timeout_ms * 1000;
};