#include "stepper_motor.h"

using namespace GPIO;

StepperMotor::StepperMotor()
{
    gpio_set_function(STEPPER_MOTOR_STEP     , PI_FUNCTION::OUTPUT);
    gpio_set_function(STEPPER_MOTOR_DIRECTION, PI_FUNCTION::OUTPUT);
<<<<<<< HEAD
    gpio_set_function(STEPPER_MOTOR_ENABLE   , PI_FUNCTION::OUTPUT);
=======
    gpio_set_function(STEPPER_MOTOR_ENABLE, PI_FUNCTION::OUTPUT);
>>>>>>> Final Code

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
        gpio_write(STEPPER_MOTOR_DIRECTION, PI_OUTPUT::LOW);
        m_target = pos * m_mircosteps_per_rev;
    }
    else
    {
        gpio_write(STEPPER_MOTOR_DIRECTION, PI_OUTPUT::HIGH);
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
<<<<<<< HEAD
    gpio_write(STEPPER_MOTOR_ENABLE, PI_OUTPUT::LOW);
=======

    gpio_write(STEPPER_MOTOR_ENABLE, PI_OUTPUT::LOW);
    stepper_done = false;
>>>>>>> Final Code

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
<<<<<<< HEAD
=======

>>>>>>> Final Code
        gpio_write(STEPPER_MOTOR_STEP, PI_OUTPUT::HIGH);
        usleep(delay);

        gpio_write(STEPPER_MOTOR_STEP, PI_OUTPUT::LOW);
        usleep(delay);

        m_stepper_pos++; 

        if (m_stepper_pos > m_target)
            break;
    }
<<<<<<< HEAD

    gpio_write(STEPPER_MOTOR_ENABLE, PI_OUTPUT::HIGH);
=======
    gpio_write(STEPPER_MOTOR_ENABLE, PI_OUTPUT::HIGH);
    stepper_done = true;
>>>>>>> Final Code
}
