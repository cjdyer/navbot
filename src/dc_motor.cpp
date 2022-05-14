#include "dc_motor.h"
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace GPIO;
using std::placeholders::_1;

Terminal DCMotors::m_terminal;

DCMotors::DCMotors()
{
    gpio_set_function(DC_MOTOR_DIRECTION_LEFT_A , PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_LEFT_B , PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_RIGHT_A, PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_RIGHT_B, PI_FUNCTION::OUTPUT);

    m_encoder = new Encoder();
    std::unique_ptr<PID> m_left_pid(new PID({0.0, 0.0, 0.0, 0}, "Left PID"));
    std::unique_ptr<PID> m_right_pid(new PID({0.0, 0.0, 0.0, 0}, "Right PID"));

    m_left_pid->set_callback(std::bind(&DCMotors::run_left_motor, this, _1));
    m_right_pid->set_callback(std::bind(&DCMotors::run_right_motor, this, _1));

    braking = false;

    Log::log_info("DCMotors::DCMotors - Motors Created");
}

DCMotors::~DCMotors()
{   
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

void DCMotors::disable() 
{ 
    braking = true; 
}

void DCMotors::enable() 
{ 
    braking = false; 
}

void DCMotors::run_left_motor(int pwm_value) 
{
    static bool direction;

    pwm_value = pwm_value * !braking;

    std::ostringstream ostr;
    ostr << (char)'L';
    ostr << std::setfill('0') << std::setw(3) << abs(pwm_value);
    m_terminal.write_serial(ostr.str().c_str());

    direction = pwm_value > 0;

    gpio_write(DC_MOTOR_DIRECTION_LEFT_A, direction);
    gpio_write(DC_MOTOR_DIRECTION_LEFT_B, direction);

    std::cout << gpio_read(DC_MOTOR_DIRECTION_LEFT_A) << gpio_read(DC_MOTOR_DIRECTION_LEFT_B) << std::endl;
}

void DCMotors::run_right_motor(int pwm_value)
{
    static bool direction;

    pwm_value = pwm_value * !braking;

    std::ostringstream ostr;
    ostr << (char)'R';
    ostr << std::setfill('0') << std::setw(3) << abs(pwm_value);
    m_terminal.write_serial(ostr.str().c_str()); 

    direction = pwm_value > 0;

    gpio_write(DC_MOTOR_DIRECTION_RIGHT_A, direction);
    gpio_write(DC_MOTOR_DIRECTION_RIGHT_B, !direction);
}