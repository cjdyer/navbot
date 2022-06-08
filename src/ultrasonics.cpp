#include "ultrasonics.h"

Ultrasonics::Ultrasonics()
{
    GPIO::gpio_set_function(ULTRA_SONIC_TRIGGER, GPIO::PI_FUNCTION::OUTPUT);

    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_N, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_E, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_S, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_W, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_NE, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_NW, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_SE, GPIO::PI_FUNCTION::INPUT);
    GPIO::gpio_set_function(ULTRA_SONIC_ECHO_SW, GPIO::PI_FUNCTION::INPUT);
}

void Ultrasonics::set_callback(ultra_callback_t callback)
{
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = callback;
}

void Ultrasonics::unset_callback()
{
    std::lock_guard<std::mutex> lk(m_callback_mutex);
    m_callback = nullptr;
}

void Ultrasonics::run_ultras()
{
    GPIO::gpio_write(ULTRA_SONIC_TRIGGER, GPIO::PI_OUTPUT::HIGH);
    usleep(11); // Should be 10 us, 11 to be safe
    GPIO::gpio_write(ULTRA_SONIC_TRIGGER, GPIO::PI_OUTPUT::LOW);

    // Might need to be AND/OR
    while (!GPIO::gpio_read(ULTRA_SONIC_ECHO_N)  && !GPIO::gpio_read(ULTRA_SONIC_ECHO_E)  &&
           !GPIO::gpio_read(ULTRA_SONIC_ECHO_S)  && !GPIO::gpio_read(ULTRA_SONIC_ECHO_W)  &&
           !GPIO::gpio_read(ULTRA_SONIC_ECHO_NE) && !GPIO::gpio_read(ULTRA_SONIC_ECHO_NW) &&
           !GPIO::gpio_read(ULTRA_SONIC_ECHO_SE) && !GPIO::gpio_read(ULTRA_SONIC_ECHO_SW)) 
    {
        m_start_time = GPIO::sys_tick();
    }

    int m_ultras_processed = 0;
    for (uint8_t i = 0; i < 8; i++) { m_stop_times[i] = 0; }
    
    while (m_ultras_processed < 8)
    {
        for (uint8_t i = 0; i < 8; i++) 
        {
            if (GPIO::gpio_read(m_pins[i]) && m_stop_times[i] == 0)
            {
                m_stop_times[i] = GPIO::sys_tick();
                m_ultras_processed++;
            }
        }
    }

    for (uint8_t i = 0; i < 8; i++) 
    { 
        double distance = (m_stop_times[i] - m_start_time) * m_distance_constant;

        if (distance <= m_uh_oh_distance)
        {
            std::lock_guard<std::mutex> lk(m_callback_mutex);
            if (m_callback)
            {
                m_callback(i);
            }
        }
    }

    usleep(20000);
}