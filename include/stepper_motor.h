#pragma once

#include <cstdint>
#include <thread>
#include "gpio.h"
#include "log.h"
#include "config.h"

class StepperMotor
{
public:
    StepperMotor();
    ~StepperMotor();

<<<<<<< HEAD
    void enable_stepper();
    void disable_stepper();

=======
>>>>>>> Final Code
    float get_pos();
    void go_to(float pos);
    bool stepper_done;

private:

    void run_stepper();

private:

    std::thread m_run_thread;

    static constexpr uint8_t m_microstep_amount = 16; // 1 / 16
    static constexpr uint8_t m_steps_per_rev = 200; // 200 steps/rev = 1.8 degs/step
    static constexpr float m_mircosteps_per_rev = m_microstep_amount * m_steps_per_rev; // 200 * 16 = 1 rev
    uint32_t m_stepper_pos;
    bool m_stepper_active = false;

    uint16_t m_target;

    uint16_t m_stepper_pos_offset;

    static constexpr float m_starting_delay = 4000; 
    static constexpr float m_ending_delay = 60; 
    static constexpr float m_steps_per_accel = 250;

    static constexpr float m_grad = (m_ending_delay - m_starting_delay) / m_steps_per_accel;
};
