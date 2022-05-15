#include "pid.h"

#define sgn(_n) ( ((_n) > 0) * 1 + ((_n) < 0) * -1 )

PID::PID(const PID_Base &pid_base)
    : m_pid_base(pid_base)
{
    m_pid_running = true;
    m_pid_thread = std::thread(&PID::pid_loop, this);

    Log::log_info("PID::PID - PID Created : " + m_pid_base.name);
}

PID::~PID() 
{
    m_pid_running = false;
    m_pid_thread.join();

    Log::log_info("PID::~PID - PID Destroyed : " + m_pid_base.name);
}

void PID::set_callback(pid_callback_t callback)
{
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = callback;
}

void PID::unset_callback()
{    
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = nullptr;
}

void PID::set_sensor_get(pid_sensor_get_t sensor_get)
{
    std::lock_guard<std::mutex> lk(m_sensor_mutex);
    m_sensor_get = sensor_get;
}

void PID::unset_sensor_get()
{
    std::lock_guard<std::mutex> lk(m_sensor_mutex);
    m_sensor_get = nullptr;
}

void PID::set_target(int32_t target)
{
    // mutex?
    m_target = target;
}

void PID::pid_loop()
{
    int32_t sensor_value, past_sensor_val, output; 
    uint64_t last_time, current_time, time_difference;
    double integral;

    while(m_pid_running)
    {
        while (true)
        {
            std::unique_lock<std::mutex> lk(m_callback_mutex);
            if(m_callback && m_sensor_get) break; // Check if we have a sensor to get and an output to give
            if(!m_pid_running) return;
            lk.unlock();
            usleep(m_timeout_us);
        }

        current_time = GPIO::sys_tick();
        time_difference = current_time - last_time;
        last_time = current_time;

        sensor_value = m_sensor_get(); 
        
        // Calculate error.
        m_error = m_target - sensor_value; 
        
        // Calculate integral (If conditions are met).
        integral += m_error * time_difference;

        if( (abs(m_error) > m_pid_base.integral_begin) || (m_error == 0) || (sgn(integral) != sgn(m_error)) )
            integral = 0;
        else if(abs(integral) > m_pid_base.integral_max)
            integral = m_pid_base.integral_max * sgn(integral);
        
        // Calculate derivative.
        m_derivative = (sensor_value - past_sensor_val) / time_difference; 
        
        // Calculate output.
        output = m_pid_base.constants.kP * m_error + 
                 m_pid_base.constants.kI * integral - 
                 m_pid_base.constants.kD * m_derivative; 

        // Restrict output to max/min.
        if (abs(output) > m_pid_base.max_output)
            output = m_pid_base.max_output * sgn(output);

        // Save previous sensor value.
        past_sensor_val = sensor_value;
        
        {
            std::lock_guard<std::mutex> lk(m_callback_mutex);
            m_callback(output); 
        }

        usleep(m_timeout_us);
    }
}