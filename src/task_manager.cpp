#include "task_manager.h"
#include <string>

TaskManager::TaskManager()
{    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
    std::unique_ptr<StepperMotor> m_stepper(new StepperMotor());
    std::unique_ptr<DCMotors>   m_dc_motors(new DCMotors());
}

TaskManager::~TaskManager()
{
    m_stepper->~StepperMotor();
    m_dc_motors->~DCMotors();
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
    m_dc_motors->set_left_target_position(100);
    sleep(4);
    m_dc_motors->disable();
    sleep(1);
}