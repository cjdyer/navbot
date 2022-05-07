#include "ultrasonic.h"
#include "log.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>


Ultra::Ultra() {
    
}

Ultra::~Ultra() {
    worker.join();
    callback_worker.join();
}

void Ultra::ultra_setcallback(callback_t set_func){
    callback = set_func;
}

// void Ultra::begin_ultrasonics(int (*func)()){
void Ultra::begin_ultrasonics(){
    callback_worker = std::thread(&Ultra::invoke, this);
}

void Ultra::setup(int trig, int echo, int num, callback_t set_func)
{
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

// want to write code that uses a thread for the run ultrasonic function below, and a main function that listens for a 'interrupt' and then does something. Using condition_variable

void Ultra::run_ultrasonic(callback_t outer_func, int num)
{   while(1)
    {
        // std::cout << processed << data << '\n';

        // we own the lock
        std::unique_lock<std::mutex> lk(m);

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
                processed = true;
                if(outer_func){
                    outer_func();
                }
                // std::cout << "this ever being called?" << data << '\n';
            }
        // std::cout << processed << data << '\n';
        // Manual unlocking is done before notifying, to avoid waking up
        // the waiting thread only to block again (see notify_one for details)
        lk.unlock();
        cv.notify_one();

        usleep(1000000);
    }
    
}

void Ultra::invoke()
{
    // std::cout << pin1, pin2 << '\n';
    
    // std::cout << "pins: " << pin1, pin2  << '\n';
    while(1){
        // wait for the worker
        {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{return processed;}); 
        }
        
        // This is where we can do other stuff in response to the ultrasonic interrupt.
        // func();
        std::cout << "here we are " << number << '\n';

        // return func(x, y);
        //

        processed = false;
    }    
}
 
void Ultra::main_test(){}







