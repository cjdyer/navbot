#pragma once

#include "async_handler.h"
#include "drive_controller.h"

enum class MachineState
{
    STOPPED = 0,
    RUNNING = 1
};

enum class CallbackType
{
    undefined = 0 // Add more here for each callback type we have
};

struct CallbackAction
{
    CallbackType type = CallbackType::undefined;
    // int encoder_value = 0;
    // If we want to pass a value between tasks we can add
    // something like seen in the above line
};
class TaskManager : public AsyncHandler<struct CallbackAction>
{
public:
    TaskManager();
    virtual ~TaskManager();

    void handle_trigger_action(struct CallbackAction& action) override;
    
private: // Functions
    void handle_states(const struct CallbackAction& action);
    void timer_callback();

private: // Variables
    MachineState m_machine_state;
    std::mutex m_state_mutex;

    DriveController m_left_drive;
};