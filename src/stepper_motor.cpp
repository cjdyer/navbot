#include "stepper_motor.h"

using namespace GPIO;

StepperMotor::StepperMotor(uint8_t step_pin, uint8_t direction_pin, uint8_t enable_pin) :
    m_stp_pin(step_pin), m_dir_pin(direction_pin), m_ena_pin(enable_pin)
{
    gpio_set_function(m_stp_pin, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_dir_pin, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_ena_pin, PI_FUNCTION::OUTPUT);

    Log::log_info("StepperMotor::StepperMotor - Stepper Motor Instance Created");
}

StepperMotor::~StepperMotor()
{
    if (m_run_thread.joinable())
        m_run_thread.join();
    Log::log_info("StepperMotor::~StepperMotor - Stepper Motor Instance Destroyed");
}

void StepperMotor::go_to(float pos)
{
    if (m_run_thread.joinable())
        m_run_thread.join();

    if (pos > 0)
    {
        gpio_write(m_dir_pin, PI_OUTPUT::LOW);
        m_target = pos * m_mircosteps_per_rev;
    }
    else
    {
        gpio_write(m_dir_pin, PI_OUTPUT::HIGH);
        m_target = -pos * m_mircosteps_per_rev;
    }
    m_stepper_pos = 0;
    m_run_thread = std::thread(&StepperMotor::run_stepper, this);
}

float StepperMotor::get_pos()
{
    return m_stepper_pos /  m_mircosteps_per_rev;
}

void StepperMotor::run_stepper()
{
    float delay;
    gpio_write(m_ena_pin, PI_OUTPUT::LOW);

    while(true)
    {
        if ((m_target - m_stepper_pos) < m_steps_per_accel)
        {
            delay =  (m_grad) * (m_target - m_stepper_pos) + m_starting_delay;
        }
        else
        {
            delay = m_grad * m_stepper_pos + m_starting_delay;
            delay = ((delay < m_ending_delay) * m_ending_delay) + (!(delay < m_ending_delay) * delay);
        }
        gpio_write(m_stp_pin, PI_OUTPUT::HIGH);
        usleep(delay);

        gpio_write(m_stp_pin, PI_OUTPUT::LOW);
        usleep(delay);

        m_stepper_pos++; 

        if (m_stepper_pos == m_target)
            break;
    }

    gpio_write(m_ena_pin, PI_OUTPUT::HIGH);
}
