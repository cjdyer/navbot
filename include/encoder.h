#pragma once

#include "log.h"
#include "gpio.h"
#include <atomic>
#include <thread>

class Encoder
{
public:
    Encoder(uint8_t a, uint8_t b);
    ~Encoder();
    
    void measure_encoder();
    long double get_speed();
    std::atomic<uint32_t> pos;
    std::atomic<long double> spd;
    
private: // Functions
    std::thread th2;
private: // Variables
    uint8_t encodeA;
    uint8_t encodeB;
    
};
