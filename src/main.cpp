#include "motor.h"

int main()
{
    GPIO::gpio_init();
    TaskManager t;
    t.run_tasks();

    return 0;
};
