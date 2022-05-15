#include "dc_motor.h"
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace GPIO;
using std::placeholders::_1;

DCMotors::DCMotors() : 
    m_left_pid(std::make_unique<PID>(left_drive_pid_base)), 
    m_right_pid(std::make_unique<PID>(right_drive_pid_base)),
    m_encoder(std::make_unique<Encoder>()),
    m_terminal(std::make_unique<Terminal>())
{
    gpio_set_function(DC_MOTOR_DIRECTION_LEFT_A , PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_LEFT_B , PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_RIGHT_A, PI_FUNCTION::OUTPUT);
    gpio_set_function(DC_MOTOR_DIRECTION_RIGHT_B, PI_FUNCTION::OUTPUT);

    m_left_pid->set_callback(std::bind(&DCMotors::run_left_motor, this, _1));
    m_right_pid->set_callback(std::bind(&DCMotors::run_right_motor, this, _1));
    m_left_pid->set_sensor_get(std::bind(&Encoder::get_left_position, m_encoder.get()));
    m_right_pid->set_sensor_get(std::bind(&Encoder::get_right_position, m_encoder.get()));

    braking = false;

    Log::log_info("DCMotors::DCMotors - Motors Created");
}

DCMotors::~DCMotors()
{
    m_left_pid->unset_callback();
    m_right_pid->unset_callback();
    m_left_pid->unset_sensor_get();
    m_right_pid->unset_sensor_get();
    
    Log::log_info("DCMotors::~DCMotors - Motors Destroyed");
}

void DCMotors::set_position_targets(int32_t left_target, int32_t right_target)
{
    m_left_pid->set_target(left_target);
    m_right_pid->set_target(right_target);
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
    pwm_value = pwm_value * !braking;

    //! TODO: Tidy this function and its pair (right motor)
    //        Creating an ostringstream ever time we call
    //        this function seems like a bad idea.
    //        Same can be said for the formatting done
    std::ostringstream ostr; 
    ostr << (char)'L';
    ostr << std::setfill('0') << std::setw(3) << abs(pwm_value);
    m_terminal->write_serial(ostr.str().c_str());

    bool direction = pwm_value > 0;

    gpio_write(DC_MOTOR_DIRECTION_LEFT_A, direction);
    gpio_write(DC_MOTOR_DIRECTION_LEFT_B, direction);

    //std::cout << gpio_read(DC_MOTOR_DIRECTION_LEFT_A) << gpio_read(DC_MOTOR_DIRECTION_LEFT_B) << std::endl;
}

void DCMotors::run_right_motor(int pwm_value)
{
    pwm_value = pwm_value * !braking;

    std::ostringstream ostr;
    ostr << (char)'R';
    ostr << std::setfill('0') << std::setw(3) << abs(pwm_value);
    m_terminal->write_serial(ostr.str().c_str()); 

    bool direction = pwm_value > 0;

    gpio_write(DC_MOTOR_DIRECTION_RIGHT_A, direction);
    gpio_write(DC_MOTOR_DIRECTION_RIGHT_B, !direction);
}