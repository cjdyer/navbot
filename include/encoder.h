#pragma once

#include "log.h"
#include "gpio.h"
#include <atomic>
#include <thread>

class Encoder
{
public:
    Encoder();
    ~Encoder();
    
    int32_t get_left_position();
    int32_t get_right_position();
    
private: // Functions

    void measure_encoder();

private: // Variables

    std::thread m_measure_thread;
    std::atomic<int32_t> m_position_left;
    std::atomic<int32_t> m_position_right;
    
    bool m_encoder_active;

    static constexpr uint8_t m_pin_left_a = 21;
    static constexpr uint8_t m_pin_left_b = 20;
    static constexpr uint8_t m_pin_right_a = 16;
    static constexpr uint8_t m_pin_right_b = 12;

    static constexpr uint16_t speed_scalar = 1000000 / 48; // Micro Seconds to Seconds / Encoder CPR
};
