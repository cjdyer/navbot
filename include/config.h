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

<<<<<<< HEAD
#define DC_MOTOR_DIRECTION_LEFT_A  26
#define DC_MOTOR_DIRECTION_LEFT_B  19
#define DC_MOTOR_DIRECTION_RIGHT_A 13
#define DC_MOTOR_DIRECTION_RIGHT_B  6

#define DC_MOTOR_ENCODER_LEFT_A    21
#define DC_MOTOR_ENCODER_LEFT_B    20
#define DC_MOTOR_ENCODER_RIGHT_A   16
#define DC_MOTOR_ENCODER_RIGHT_B   12

#define STEPPER_MOTOR_ENABLE       13
#define STEPPER_MOTOR_DIRECTION    19
#define STEPPER_MOTOR_STEP          6
=======
#define DC_MOTOR_DIRECTION_LEFT_A   6
#define DC_MOTOR_DIRECTION_LEFT_B  13
#define DC_MOTOR_DIRECTION_RIGHT_A 20
#define DC_MOTOR_DIRECTION_RIGHT_B 19

#define DC_MOTOR_ENCODER_LEFT_A     0
#define DC_MOTOR_ENCODER_LEFT_B     5
#define DC_MOTOR_ENCODER_RIGHT_A    9
#define DC_MOTOR_ENCODER_RIGHT_B   11

#define STEPPER_MOTOR_DIRECTION    27
#define STEPPER_MOTOR_STEP         17
#define STEPPER_MOTOR_ENABLE       22

#define ULTRA_SONIC_TRIGGER         4
#define ULTRA_SONIC_ECHO_N         18
#define ULTRA_SONIC_ECHO_E         23
#define ULTRA_SONIC_ECHO_S         24
#define ULTRA_SONIC_ECHO_W         25
#define ULTRA_SONIC_ECHO_NE         8
#define ULTRA_SONIC_ECHO_NW         7
#define ULTRA_SONIC_ECHO_SE         1
#define ULTRA_SONIC_ECHO_SW        16
>>>>>>> Final Code

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

<<<<<<< HEAD
    uint16_t max_output, integral_max, integral_begin;
};

#define DC_MOTOR_PID_INTEGRAL_MAX 9000
#define DC_MOTOR_PID_INTEGRAL_BEGIN 500
#define DC_MOTOR_PID_MAX_OUTPUT 255

static PID_Base left_drive_pid_base = { {0.0, 0.0, 0.0}, "Left Drive PID", 
    DC_MOTOR_PID_MAX_OUTPUT, DC_MOTOR_PID_INTEGRAL_MAX, DC_MOTOR_PID_INTEGRAL_BEGIN };
    
static PID_Base right_drive_pid_base = { {0.0, 0.0, 0.0}, "Right Drive PID", 
    DC_MOTOR_PID_MAX_OUTPUT, DC_MOTOR_PID_INTEGRAL_MAX, DC_MOTOR_PID_INTEGRAL_BEGIN };
=======
    uint32_t max_output, integral_max, integral_begin;
};

#define DC_MOTOR_PID_INTEGRAL_MAX   9000
#define DC_MOTOR_PID_INTEGRAL_BEGIN 3000
#define DC_MOTOR_PID_MAX_OUTPUT      130

>>>>>>> Final Code
