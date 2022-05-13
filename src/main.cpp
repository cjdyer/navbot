#include "task_manager.h"

int main()
{
    GPIO::gpio_init();
    TaskManager t;
    t.run_tasks();

    // int counter = 0;
    // int aState;
    // int aLastState;

    // GPIO::gpio_set_function(21, GPIO::PI_FUNCTION::INPUT);
    // GPIO::gpio_set_function(20, GPIO::PI_FUNCTION::INPUT);

    // // Reads the initial state of the outputA
    // aLastState = GPIO::gpio_read(21);

    // while (true)
    // {
    //     aState = GPIO::gpio_read(21); // Reads the "current" state of the outputA
    //     // If the previous and the current state of the outputA are different, that means a Pulse has occured
    //     if (aState != aLastState)
    //     {
    //         // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    //         if (GPIO::gpio_read(20) != aState)
    //         {
    //             counter++;
    //         }
    //         else
    //         {
    //             counter--;
    //         }
    //         std::cout << "Position : " << counter << std::endl;
    //     }
    //     aLastState = aState; // Updates the previous state of the outputA with the current state
    // }

    return 0;
};
