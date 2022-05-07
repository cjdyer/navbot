#pragma once
#include "gpio.h"
#include <thread>
#include <condition_variable>
#include "log.h"
#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>

class Ultra
{
public:
    Ultra();
    ~Ultra();
    using callback_t = std::function<void (void)>;
    void setup(int trig, int echo, int num, callback_t set_func);
    void run_ultrasonic(callback_t set_func, int num);
    void main_test();
    void invoke();
    void begin_ultrasonics();
    void ultra_setcallback(callback_t set_func);
    int pulse = 10;
    int level = 0;
    int m_trig;
    int m_echo;
    int number;

private: // Functions
    std::thread worker;
    std::thread callback_worker;
     
    std::mutex m;
    std::condition_variable cv;
    std::string data;
    bool ready = false;
    bool processed = false;
    callback_t callback;

}; // class Auton