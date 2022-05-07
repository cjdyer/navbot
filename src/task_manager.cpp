#include "task_manager.h"
#include <string>

TaskManager::TaskManager()
{    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
    // stepper = new StepperMotor(step_pin, dir_pin, enable_pin);
    dc_motor = new Motor(26, 19, 13, 6);
}

TaskManager::~TaskManager()
{
    // stepper->~StepperMotor();
    dc_motor->~Motor();
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
}