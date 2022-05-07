#include "task_manager.h"
#include <string>

TaskManager::TaskManager()
{    
    stepper = new StepperMotor(step_pin, dir_pin, enable_pin);
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
}

TaskManager::~TaskManager()
{
    stepper->~StepperMotor();
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{

}