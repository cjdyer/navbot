#include "pid.h"

#define sgn(_n) ( ((_n) > 0) * 1 + ((_n) < 0) * -1 )

PID::PID(const PIDConstants &pid_constants, const char* name)
    : m_pid_constants(pid_constants), m_name(name) 
{
    m_pid_running = true;
    m_pid_thread = std::thread(&PID::pid_loop, this);
}

PID::~PID() 
{
    m_pid_running = false;
    m_pid_thread.join();
}

void PID::set_callback(pid_callback_t _callback)
{
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = _callback;
}

void PID::unset_callback()
{    
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = nullptr;
}

void PID::set_target(int32_t target)
{
    // mutex?
    m_target = target;
}

void PID::pid_loop()
{
    static double past_sensor_val, integral, output;
    static uint64_t last_time, current_time, time_difference;
    double sensor_value = 0.0; // m_sensor.get();

    while(m_pid_running)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lk(m_callback_mutex);
            if(m_callback) break;
            lk.unlock();
            usleep(m_timeout_us);
        }

        current_time = GPIO::sys_tick();
        time_difference = current_time - last_time;
        last_time = current_time;
        
        m_error = m_target - sensor_value; // Calculate error.
        
        // Calculate integral (If conditions are met).
        integral += m_error * time_difference;

        if( (abs(m_error) > m_pid_constants.integral_begin) || (m_error == 0) || (sgn(integral) != sgn(m_error)) )
            integral = 0;
        else if(abs(integral) > m_pid_constants.integral_max)
            integral = m_pid_constants.integral_max * sgn(integral);
        
        m_derivative = (sensor_value - past_sensor_val) / time_difference; // Calculate derivative.
        
        output = m_pid_constants.kP * m_error + m_pid_constants.kI * integral - m_pid_constants.kD * m_derivative; // Calculate output.

        // Restrict output to max/min.
        if (abs(output) > m_pid_constants.max_output)
            output = m_pid_constants.max_output * sgn(output);

        // Save previous sensor value.
        past_sensor_val = sensor_value;
        
        {
            std::lock_guard<std::mutex> lk(m_callback_mutex);
            if(m_callback) { m_callback(output); }
        }
        
        usleep(m_timeout_us);
    }
}