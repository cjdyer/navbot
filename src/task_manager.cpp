#include "task_manager.h"
#include <string>

TaskManager::TaskManager() : 
    m_stepper(std::make_unique<StepperMotor>()),
    m_dc_motors(std::make_unique<DCMotors>())
{    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
}

TaskManager::~TaskManager()
{
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
    m_dc_motors->disable();
    sleep(1);
}