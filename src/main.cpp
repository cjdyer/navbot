#include "motor.h"

int main()
{
    std::cout << "start" << std::endl;
    Motor m(1,2,3,4);  
    m.run_with_pid_control(5);  // run the motor with a target speed
    return 0;
};
