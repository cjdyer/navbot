#pragma once

#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>
#include "gpio.h"
#include "encoder.h"
#include "terminal.h"

class DCMotors
{
public: 
    DCMotors();
    ~DCMotors();

    void disable(bool drive_side);
    void enable(bool drive_side);

    void set_left_target_position(int32_t target);
    void set_right_target_position(int32_t target);

private:

    void pid_loop();

    void run_left_motor(bool direction, int pwm_val);
    void run_right_motor(bool direction, int pwm_val); 

    bool left_braking;
    bool right_braking;

    static constexpr uint8_t m_pin_dir_left_a = 26;
    static constexpr uint8_t m_pin_dir_left_b = 19;
    static constexpr uint8_t m_pin_dir_right_a = 13;
    static constexpr uint8_t m_pin_dir_right_b = 6;

    uint16_t m_encoder_value;
    Encoder *m_encoder;

    // PID constants
    static constexpr float kp = 1;
    static constexpr float kd = 0;
    static constexpr float ki = 0;
  
    static Terminal m_terminal;

    bool m_pid_running;
    int32_t m_target_left;
    int32_t m_target_right;
    std::thread m_pid_thread;
};