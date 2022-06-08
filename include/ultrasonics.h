#ifndef ULTRASONICS_H
#define ULTRASONICS_H

#include "config.h"
#include "gpio.h"
#include <thread>
#include <functional>
#include <mutex>
#include <vector>

using ultra_callback_t   = std::function<void(int)>;

class Ultrasonics
{
    Ultrasonics();
    ~Ultrasonics() {}

    void set_callback(ultra_callback_t callback);
    void unset_callback();

private:

    void run_ultras();

    std::thread m_ultra_thread;
    bool m_ultras_running;

    uint64_t m_start_time;
    uint8_t m_pins[8] = { ULTRA_SONIC_ECHO_N,  ULTRA_SONIC_ECHO_E,  ULTRA_SONIC_ECHO_S,  ULTRA_SONIC_ECHO_W,
                          ULTRA_SONIC_ECHO_NE, ULTRA_SONIC_ECHO_NW, ULTRA_SONIC_ECHO_SE, ULTRA_SONIC_ECHO_SW  };
    uint64_t m_stop_times [8];
    double m_distances [8];

    ultra_callback_t m_callback;
    std::mutex m_callback_mutex;

    static constexpr double m_distance_constant = 34300.0 / 2.0 / 1000000.0;
    static constexpr float m_uh_oh_distance = 1.0;
};

#endif 