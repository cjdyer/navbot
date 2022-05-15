#pragma once

#include <string>

/**********************************
 * 
 *     SERIAL COMMUNICATION
 * 
**********************************/

static std::string serial_port = "/dev/ttyUSB0";

/**********************************
 * 
 *        GPIO PINS
 * 
**********************************/

#define DC_MOTOR_DIRECTION_LEFT_A  26
#define DC_MOTOR_DIRECTION_LEFT_B  19
#define DC_MOTOR_DIRECTION_RIGHT_A 13
#define DC_MOTOR_DIRECTION_RIGHT_B  6

#define DC_MOTOR_ENCODER_LEFT_A  21
#define DC_MOTOR_ENCODER_LEFT_B  20
#define DC_MOTOR_ENCODER_RIGHT_A 16
#define DC_MOTOR_ENCODER_RIGHT_B 12

#define STEPPER_MOTOR_ENABLE       13
#define STEPPER_MOTOR_DIRECTION    19
#define STEPPER_MOTOR_STEP          6

/**********************************
 * 
 *        PID CONFIGS
 * 
**********************************/

// Base Model for PID's
struct PID_Base
{
    struct
    {
        float kP, kI, kD;
    } constants;

    const std::string name;

    uint16_t max_output, integral_max, integral_begin;
};

#define DC_MOTOR_PID_INTEGRAL_MAX 9000
#define DC_MOTOR_PID_INTEGRAL_BEGIN 500
#define DC_MOTOR_PID_MAX_OUTPUT 255

static PID_Base left_drive_pid_base = { {0.0, 0.0, 0.0}, "Left Drive PID", 
    DC_MOTOR_PID_MAX_OUTPUT, DC_MOTOR_PID_INTEGRAL_MAX, DC_MOTOR_PID_INTEGRAL_BEGIN };
    
static PID_Base right_drive_pid_base = { {0.0, 0.0, 0.0}, "Right Drive PID", 
    DC_MOTOR_PID_MAX_OUTPUT, DC_MOTOR_PID_INTEGRAL_MAX, DC_MOTOR_PID_INTEGRAL_BEGIN };
