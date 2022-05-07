#include "task_manager.h"

int main(int argc, char const *argv[])
{
    GPIO::gpio_init();
    TaskManager t;
    t.run_tasks();
    return 0;
}
