#pragma once

#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>
#include "gpio.h"

class Motor
{
public: 
    Motor(uint8_t pin_dir, uint8_t pin_pwm, uint8_t pin_enc_a, uint8_t pin_enc_b);
    ~Motor();

    void encoder_position();
    void run_with_pid_control(int target_speed);
    void runMotor(int dir, int pwmVal);

private:
    // std::atomic<uint32_t> pos;
    std::thread th;
    int position;
    long double speed;
    float pwr;  

    const uint8_t m_pin_dir;
    const uint8_t m_pin_pwm;
    const uint8_t m_pin_enc_a;
    const uint8_t m_pin_enc_b;

    uint16_t m_encoder_value;

    // PID constants
    static constexpr float kp = 1;
    static constexpr float kd = 0.025;
    static constexpr float ki = 0.0;
     
};