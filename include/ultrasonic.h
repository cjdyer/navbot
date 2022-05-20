#pragma once
#include "gpio.h"
#include <thread>
#include <condition_variable>
#include "log.h"
#include <iostream>
#include <string>
#include <functional>

class Ultra
{
public:
    using callback_t = std::function<void (void)>;
    Ultra(int trig, int echo, int num, callback_t set_func);
    ~Ultra();
    void run_ultrasonic(callback_t set_func, int num);
    int pulse = 10;
    int level = 0;
    int m_trig;
    int m_echo;
    int number;

private: // Functions
    std::thread worker;
    std::string data;
    callback_t callback;

};