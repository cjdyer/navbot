#pragma once

#include "log.h"
#include "stepper_motor.h"

class TaskManager
{
public:
    TaskManager();
    ~TaskManager();
    
    void run_tasks();
private: // Functions

private: // Variables
    static constexpr uint8_t enable_pin = 13;
    static constexpr uint8_t dir_pin = 19;
    static constexpr uint8_t step_pin = 6;

    StepperMotor *stepper;
};