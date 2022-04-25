#pragma once

#include <string>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

struct Log
{
    static std::string get_time_stamp()
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        std::string s(20, '\0');
        std::strftime(&s[0], s.size(), "%d-%m-%Y %X", std::localtime(&now));
        return s;
    }

    static void log_info(const std::string& msg)
    {
        std::cout << get_time_stamp() << " " << msg.c_str() << std::endl << std::flush;
    }
}; // struct Log