#pragma once

#include "gpio.h"
#include "config.h"
#include "terminal.h"
#include "pid.h"
#include "config.h"

class DCMotors
{
public: 
    DCMotors();
    ~DCMotors();

<<<<<<< HEAD
    void disable();
    void enable();

    void set_position_targets(int32_t left_target, int32_t right_target);

private:

    void run_left_motor(int pwm_val);
    void run_right_motor(int pwm_val); 

private:

    bool braking;

    int32_t m_target_left; 
    int32_t m_target_right;

    std::unique_ptr<PID> m_left_pid;
    std::unique_ptr<PID> m_right_pid;
    std::unique_ptr<Terminal> m_terminal;
    std::unique_ptr<Encoder> m_encoder;
=======
    void run_left_motor(int pwm_val);
    void run_right_motor(int pwm_val); 

private:

    int32_t m_target_left; 
    int32_t m_target_right;

    std::unique_ptr<Terminal>      m_terminal;
>>>>>>> Final Code
};