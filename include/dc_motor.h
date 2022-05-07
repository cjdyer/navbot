#pragma once

#include <cstdint>
#include "gpio.h"

class DCMOTOR
{

public:
    
    DCMOTOR(uint8_t pin_dir, uint8_t pin_pwm, uint8_t pin_enc_a, uint8_t pin_enc_b);
    ~DCMOTOR() {}

    void set_speed(uint8_t speed);
    void enable();
    void disable();

    int64_t get_position();
    void set_position(int64_t position);
    void brake_mode_active();

    void debug();

private:

    const uint8_t m_pin_dir;
    const uint8_t m_pin_pwm;
    const uint8_t m_pin_enc_a;
    const uint8_t m_pin_enc_b;

    uint16_t m_encoder_value;

}; // class DCMOTOR
