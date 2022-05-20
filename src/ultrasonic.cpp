#include "ultrasonic.h"
#include "log.h"
#include <iostream>
#include <string>
#include <thread>

Ultra::Ultra(int trig, int echo, int num, callback_t set_func) {
    callback = set_func;
    // pin 15 - TRIG
    // pin 18 - ECHO
    number = num;
    GPIO::gpioSetMode(trig, PI_OUTPUT);
    GPIO::gpioSetMode(echo, PI_INPUT);
    m_trig = trig;
    m_echo = echo;
    std::cout << "pins: " << m_trig << " and " << m_echo << " successfully setup" << '\n';
    worker = std::thread(&Ultra::run_ultrasonic, this, std::ref(callback), number);
}

Ultra::~Ultra() {
    worker.join();
}

void Ultra::run_ultrasonic(callback_t outer_func, int num)
{   while(1)
    {

        GPIO::gpioTrigger(m_trig, pulse, level);
        int StartTime, StopTime;

        while(GPIO::gpioRead(m_echo) == 0)
        {
            StartTime = GPIO::gpioTick();
        }
    
        while(GPIO::gpioRead(m_echo) == 1)
        {
            StopTime = GPIO::gpioTick();
        }
    
        int TimeElapsed = StopTime - StartTime;
    
        double distance = (TimeElapsed * 34300.0) / 2.0 / 1000000;

        Log::log_info("Measured distance for sensor " + std::to_string(number) + " = " +  std::to_string(distance));

        // set a rule that if the distance is less than 10cm then send notification to main_test

        if(distance<=10.0)
            {
                if(outer_func){
                    outer_func();
                }           
            }

        usleep(1000000);
    }
}