#include "dc_motor.h"
#include "encoder.h"
#include <chrono>

using namespace GPIO;

Motor::Motor(uint8_t pin_dir, uint8_t pin_pwm, uint8_t pin_enc_a, uint8_t pin_enc_b) : m_pin_dir(pin_dir), m_pin_pwm(pin_pwm), m_pin_enc_a(pin_enc_a), m_pin_enc_b(pin_enc_b)
{
    gpio_set_function(m_pin_dir, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_pin_pwm, PI_FUNCTION::ALT_0);
    gpio_set_function(m_pin_enc_a, PI_FUNCTION::INPUT);
    gpio_set_function(m_pin_enc_b, PI_FUNCTION::INPUT); // get a segmentation fault when included...

    position = 0;

    pwr = 0; // start the pwd signal at 0 and increase to the required speed.
    pwm_start(m_pin_pwm);

    std::cout << "constructor" << std::endl;
    th = std::thread(&Motor::encoder_position, this);
}

Motor::~Motor()
{
    th.join();
    pwm_stop();
}

void Motor::encoder_position() // Function to grab position data from encoder code, entered into thread to run continuously.
{
    Encoder E(m_pin_enc_a, m_pin_enc_b);
    while (1)
    {
        speed = E.get_speed();
        usleep(100000);
    }

}

void Motor::run_with_pid_control(int target_speed) // Function to grab position data from encoder code, entered into thread to run continuously.
{
    float eintegral = 0;
    float eprev = 0;
    std::chrono::_V2::system_clock::time_point prevT;

    while (1)
    {
        // time diff for delta
        auto start = std::chrono::high_resolution_clock::now();
        double microseconds = start.time_since_epoch().count() - prevT.time_since_epoch().count();
        // error
        float error = target_speed - speed;
        // derivative
        float derivative = (error - eprev) / microseconds;
        // integral
        eintegral += error * microseconds;

        // control signal
        float u = kp * error + kd * derivative + ki * eintegral;

        // motor power
        pwr += u;
        
        if (pwr > 255)
        { 
            pwr = 255;
        }
        else if (pwr < 0)
        {
            pwr = 0;
        }

        // motor direction needs to be set based on direction of current travel, established from speed direction.
        int dir = 1;
        if (speed < 0)
        {
            dir = -1;
        }

        std::cout << "speed " << speed << std::endl;
        std::cout << "u " << u << std::endl;
        std::cout << "pwr " << pwr << std::endl;
        std::cout << "dir " << dir << std::endl;

        // Finally run the motor at the calculated speed and direction from PID calcs.
        runMotor(dir,pwr);  // Segmentation fault atm

        // sleep abit to slow PID down to reasonable measurements
        usleep(10000);
        
        eprev = error;
        prevT = std::chrono::high_resolution_clock::now();
    }
}

void Motor::runMotor(int dir, int pwmVal) // function to set the motor running
{
    const uint8_t input_pwm_period = pwmVal*20/255;
    pwm_write(20, pwmVal);   // analogue write gpio function is needed
    if (dir == 1)
    {
        gpio_write(m_pin_dir, PI_OUTPUT::HIGH);
    }
    else
    {
        gpio_write(m_pin_dir, PI_OUTPUT::LOW);
    }
}
