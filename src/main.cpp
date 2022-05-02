#include "task_manager.h"

int main(int argc, char const *argv[])
{
    
    // Code to run a PWM 
    
    GPIO::gpio_init();

    GPIO::gpio_set_function(18, GPIO::PI_FUNCTION::ALT_5); // Set Function to pwm
    GPIO::gpio_set_function(19, GPIO::PI_FUNCTION::ALT_5); // Set Function to pwm

    std::cout << "CHANNEL 1" << std::endl;
    GPIO::pwm_start(GPIO::PI_PWM_CHANNEL::CHA_1);
    GPIO::pwm_write(1024);
    sleep(10);

    // for (size_t i = 0; i < 10; i++)
    // {
    //     std::cout << GPIO::gpio_read(18) << " " << GPIO::gpio_read(19) << std::endl;
    //     usleep(1);
    // }

    GPIO::pwm_stop(GPIO::PI_PWM_CHANNEL::CHA_1);

    // std::cout << "CHANNEL 2" << std::endl;
    // GPIO::pwm_start();

    // for (size_t i = 0; i < 10; i++)
    // {
    //     std::cout << GPIO::gpio_read(18) << " " << GPIO::gpio_read(19) << std::endl;
    //     usleep(1);
    // }
    // GPIO::pwm_stop();

    // TaskManager t;
    // t.run_tasks();
    return 0;
}
