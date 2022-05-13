#include "task_manager.h"
#include <string>

TaskManager::TaskManager()
{    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
    // stepper = new StepperMotor(step_pin, dir_pin, enable_pin);
    dc_motors = new DCMotors();
}

TaskManager::~TaskManager()
{
    // stepper->~StepperMotor();
    dc_motors->~DCMotors();
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
    dc_motors->set_left_target_position(100);
    sleep(4);
    dc_motors->disable(true);
    dc_motors->disable(false);
    sleep(1);
}