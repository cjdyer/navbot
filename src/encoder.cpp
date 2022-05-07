#include "encoder.h"
#include <string>

using namespace GPIO;

Encoder::Encoder(uint8_t pin_a, uint8_t pin_b) : m_pin_a(pin_a), m_pin_b(pin_b)
{
    gpio_set_function(m_pin_a, PI_FUNCTION::INPUT); // ENC A
    gpio_set_function(m_pin_b, PI_FUNCTION::INPUT); // ENC B

    m_position.store(0);
    m_speed.store(0);

    m_encoder_active = true;
    m_measure_thread = std::thread(&Encoder::measure_encoder, this);

    Log::log_info("Encoder::Encoder - Encoder instance created");
}

Encoder::~Encoder()
{
    m_encoder_active = false;
    m_measure_thread.join();
    Log::log_info("Encoder::Encoder - Encoder instance destroyed");
}

double Encoder::get_speed()
{
    return m_speed.load();
}

void Encoder::measure_encoder()
{
    static uint32_t current_time, elapsed_time, last_time= sys_tick();
    static bool current_state, last_state = gpio_read(m_pin_a);
    static double speed;

    while (m_encoder_active)
    {
        current_state = gpio_read(m_pin_a);
        current_time = sys_tick();
        elapsed_time = current_time - last_time;

        // Could make branchless??
        if (gpio_read(m_pin_b) != current_state)
        {
            m_position--;
            speed = -1.0;
        }
        else
        {
            m_position++;
            speed = 1.0;
        }

        speed *= (speed_scalar / elapsed_time);

        m_speed.store(speed);

        last_time = current_time;
        last_state = current_state;
        usleep(1000);
    }
}
