#include "task_manager.h"

TaskManager::TaskManager() : AsyncHandler()
{
    m_left_drive.set_timer_callback(std::bind(&TaskManager::timer_callback, this));
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
}

TaskManager::~TaskManager()
{
    stop_handler(); // Stop async handler
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::handle_trigger_action(struct CallbackAction& action)
{   
    handle_states(action);
    Log::log_info("TaskManager::handle_trigger_action - Action triggered");
}

void TaskManager::timer_callback()
{
    //Check that the drive is working
}

void TaskManager::handle_states(const struct CallbackAction& action)
{
    // State machine goes here
}