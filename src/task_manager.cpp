#include "task_manager.h"
#include <string>

using namespace GPIO;

TaskManager::TaskManager()
{
    gpio_set_function(13, PI_FUNCTION::OUTPUT);
    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
}

TaskManager::~TaskManager()
{
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 2000;

    while (1)
    {
        gpio_write(13, (bool)PI_OUTPUT::HIGH);
        nanosleep(&delay, NULL);
        gpio_write(13, (bool)PI_OUTPUT::LOW);
        nanosleep(&delay, NULL);
    }
    
    // gpio_set_function(5, PI_FUNCTION::INPUT); // ENC B
    // gpio_set_function(6, PI_FUNCTION::INPUT); // ENC A
    // int counter = 0;
    // bool current_state_A;
    // bool last_state_A = gpio_read(6);
    // std::string currentDir = "";
    // double speed;
    // uint32_t last_time = sys_tick();
    // uint32_t current_time;

    // while(1)
    // {
    //     current_state_A = gpio_read(6);

    //     if (current_state_A != last_state_A  && current_state_A == 1)
    //     {
    //         current_time = sys_tick();

    //         // If the DT state is different than the CLK state then
    //         // the encoder is rotating CCW so decrement
    //         if (gpio_read(5) != current_state_A) 
    //         {
    //             counter --;
    //             speed = - 1 /  current_time - last_time;
    //             currentDir ="CCW";
    //         } 
    //         else 
    //         {
    //             // Encoder is rotating CW so increment
    //             counter ++;
    //             speed = 1 /  current_time - last_time;
    //             currentDir ="CW";
    //         }

    //         last_time = sys_tick();

    //         speed /= 48;
    //         speed /= 1000000;

    //         std::cout << "Direction: " << currentDir << " | Counter: " << counter << " | Speed: " << speed << std::endl;
    //     }

    //     // Remember last CLK state
    //     last_state_A = current_state_A;
    //     // Put in a slight delay to help debounce the reading
    //     usleep(1000);
    // }
}