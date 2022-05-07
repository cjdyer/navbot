#include "encoder.h"
#include <string>

using namespace GPIO;

Encoder::Encoder(uint8_t a, uint8_t b): encodeA(a), encodeB(b)
{
    // gpio_set_function(13, PI_FUNCTION::OUTPUT);
    pos.store(0);
    spd.store(0);
    th2 = std::thread(&Encoder::measure_encoder, this);
    // Log::log_info("Started Encoder");
}

Encoder::~Encoder()
{
    // Log::log_info("Encoder terminated");
    th2.join();
}

long double Encoder::get_speed()
{
    // std::cout << pos.load() << std::endl;
    return spd.load();
}

void Encoder::measure_encoder()
{
    gpio_set_function(encodeA, PI_FUNCTION::INPUT); // ENC B
    gpio_set_function(encodeB, PI_FUNCTION::INPUT); // ENC A
    int counter = pos.load();
    bool current_state_A;
    // bool last_state_A = 0;
    bool last_state_A = gpio_read(6);
    std::string currentDir = "";
    double speed;
    auto last_time = std::chrono::high_resolution_clock::now();
    // uint32_t last_time = sys_tick();
    // uint32_t current_time;
    

    while(1)
    {
        

        // current_state_A = 0;
        current_state_A = gpio_read(6);

        if (1 == 1)
        {
            usleep(4080); // need this to avoid divide by zero error in first elapsed time comparison.
            // current_time = sys_tick();
            auto current_time = std::chrono::high_resolution_clock::now();

            auto elapsed = current_time - last_time;
            long double time_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            // std::cout << "time_elapsed " << time_elapsed << std::endl;
            // If the DT state is different than the CLK state then
            // the encoder is rotating CCW so decrement
            // if (0 != current_state_A)
            if (gpio_read(5) != current_state_A)  
            {
                counter --;
                speed = - 1.0 /  time_elapsed;
                currentDir ="CCW";
            } 
            else 
            {
                // Encoder is rotating CW so increment
                counter ++;
                speed = 1.0 / time_elapsed;
                currentDir ="CW";
            }

            last_time = std::chrono::high_resolution_clock::now();

            speed /= 48;    // what is this?
            speed *= 1000000;

            // std::cout << "speed " << speed << std::endl;

            // std::cout << "Direction: " << currentDir << " | Counter: " << counter << " | Speed: " << speed << std::endl;
            
            pos.store(counter);
            spd.store(speed);
        }

        // Remember last CLK state
        last_state_A = current_state_A;
        // Put in a slight delay to help debounce the reading
        usleep(1000);
    }
}
