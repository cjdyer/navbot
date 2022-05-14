#pragma once

#include <cstdint>
#include <atomic>
#include <thread>
#include <iostream>
#include <unistd.h>
#include "gpio.h"
#include "encoder.h"
#include "terminal.h"
#include "pid.h"
#include "config.h"

class DCMotors
{
public: 
    DCMotors();
    ~DCMotors();

    void disable();
    void enable();

    void set_left_target_position(int32_t target);
    void set_right_target_position(int32_t target);

private:


    void run_left_motor(int pwm_val);
    void run_right_motor(int pwm_val); 

    bool braking;

    uint16_t m_encoder_value;
    Encoder *m_encoder;
  
    static Terminal m_terminal;

    int32_t m_target_left;
    int32_t m_target_right;

    std::unique_ptr<PID> m_left_pid;
    std::unique_ptr<PID> m_right_pid;
};