#pragma once

#include "log.h"
#include "gpio.h"
#include <atomic>
#include <thread>

class Encoder
{
public:
    Encoder(uint8_t pin_a, uint8_t pin_b);
    ~Encoder();
    
    double get_speed();
    
private: // Functions
    std::thread m_measure_thread;
    std::atomic<double> m_speed;
    std::atomic<uint32_t> m_position;
    bool m_encoder_active;

    void measure_encoder();

private: // Variables
    const uint8_t m_pin_a;
    const uint8_t m_pin_b;

    static constexpr uint16_t speed_scalar = 1000000 / 48; // Micro Seconds to Seconds / Encoder CPR
};
