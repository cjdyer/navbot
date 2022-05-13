#include "dc_motor.h"
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace GPIO;

Terminal DCMotors::m_terminal;

DCMotors::DCMotors()
{
    gpio_set_function(m_pin_dir_left_a, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_pin_dir_left_b, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_pin_dir_right_a, PI_FUNCTION::OUTPUT);
    gpio_set_function(m_pin_dir_right_b, PI_FUNCTION::OUTPUT);

    m_encoder = new Encoder();

    left_braking = false;
    right_braking = false;

    m_pid_running = true;
    m_pid_thread = std::thread(&DCMotors::pid_loop, this);

    Log::log_info("DCMotors::DCMotors - Motors Created");
}

DCMotors::~DCMotors()
{   
    m_pid_running = false;
    m_pid_thread.join();
    m_encoder->~Encoder();
    Log::log_info("DCMotors::~DCMotors - Motors Destroyed");
}

void DCMotors::set_left_target_position(int32_t target)
{
    m_target_left = target;
}

void DCMotors::set_right_target_position(int32_t target)
{
    m_target_right = target;
}

void DCMotors::disable(bool drive_side)
{
    if (drive_side)
        left_braking = true;
    else
        right_braking = true; 
}

void DCMotors::enable(bool drive_side)
{ 
    if (drive_side)
        left_braking = false;
    else
        right_braking = false; 
}

void DCMotors::pid_loop() // Function to grab position data from encoder code, entered into thread to run continuously.
{
    float integral = 0;
    float previous_error = 0;
    uint32_t time_current, time_previous;

    while (m_pid_running)
    {
        // time diff for delta
        uint32_t time_start = sys_tick();
        double microseconds = time_previous - time_current;
        time_previous = time_start;

        // error
        float error = m_target_left- m_encoder->get_left_position();
        //std::cout << error << std::endl;
        // derivative
        float derivative = (error - previous_error) / microseconds;
        // integral
        integral += error * microseconds;

        // control signal
        float output = kp * error; //+ kd * derivative + ki * integral;

        // motor direction needs to be set based on direction of current travel, established from speed direction.
        int dir = 1;

        if (output < 0)
        {
            output = -output;
            dir = -1;
        }

        if (output == -0) // Not sure if this clause is needed, should check at some point
        {
            output = 0;
        }

        Log::log_info("DCMotors::pid_loop - Output : " + std::to_string(output));

        // Finally run the motor at the calculated speed and direction from PID calcs.
        run_left_motor(dir, output);
        run_left_motor(dir, 0);

        // sleep abit to slow PID down to reasonable measurements
        usleep(10000);
        
        previous_error = error;
    }
}

void DCMotors::run_left_motor(bool direction, int pwm_value) // function to set the motor running
{
    pwm_value = pwm_value * !left_braking;

    std::ostringstream ostr;
    ostr << (char)'L';
    ostr << std::setfill('0') << std::setw(3) << pwm_value;
    m_terminal.write_serial(ostr.str().c_str());

    gpio_write(m_pin_dir_left_a, direction);
    gpio_write(m_pin_dir_left_b, direction);

    std::cout << gpio_read(m_pin_dir_left_a) << gpio_read(m_pin_dir_left_b) << std::endl;

}
void DCMotors::run_right_motor(bool direction, int pwm_value) // function to set the motor running
{

    pwm_value = pwm_value * !right_braking;

    std::ostringstream ostr;
    ostr << (char)'R';
    ostr << std::setfill('0') << std::setw(3) << pwm_value;
    m_terminal.write_serial(ostr.str().c_str()); 

    gpio_write(m_pin_dir_right_a, direction);
    gpio_write(m_pin_dir_right_b, !direction);
}