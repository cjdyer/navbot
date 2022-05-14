#pragma once

#include "log.h"
#include "stepper_motor.h"
#include "dc_motor.h"
#include "terminal.h"

class TaskManager
{
public:
    TaskManager();
    ~TaskManager();
    
    void run_tasks();
private: // Functions

private: // Variables

    std::unique_ptr<StepperMotor>   m_stepper;
    std::unique_ptr<DCMotors>     m_dc_motors;
};