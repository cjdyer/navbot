#include "dc_motor.h"

using namespace GPIO;

DCMOTOR::DCMOTOR(uint8_t pin_dir, uint8_t pin_pwm, uint8_t pin_enc_a, uint8_t pin_enc_b) :
    m_pin_dir(pin_dir), m_pin_pwm(pin_pwm), m_pin_enc_a(pin_enc_a), m_pin_enc_b(pin_enc_b)
{
    gpio_set_function(m_pin_dir, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_pin_pwm, PI_FUNCTION::ALT_0);
    gpio_set_function(m_pin_enc_a, PI_FUNCTION::INPUT);
    gpio_set_function(m_pin_enc_b, PI_FUNCTION::INPUT);
}

void DCMOTOR::set_speed(uint8_t speed)
{
    pwm0_write(speed, false);
}

void DCMOTOR::enable()
{
    p
}

void disable();