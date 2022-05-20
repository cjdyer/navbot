#pragma once

#include <cstdint>

#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "config.h"

class Terminal
{
public:
    Terminal();
    ~Terminal();

    void start_polling();
    void stop_polling();

    uint8_t read_serial(char (&_output)[100]);
    void write_serial(const char* _msg);

private:
    void run_polling();

private:
    struct termios m_config;
    int m_serial_port;
    const std::string m_port = serial_port;

    std::mutex m_serial_mutex;
    std::thread m_handler_thread;
    std::atomic<bool> m_polling_condition;

    bool m_response_recieved;
    std::mutex m_writable_mutex;
    std::condition_variable m_writable;
};

