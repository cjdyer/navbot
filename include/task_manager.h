#pragma once

#include "log.h"
#include "stepper_motor.h"
#include "dc_motor.h"
#include "pure_pursuit.h"
#include "pid.h"
#include "encoder.h"
#include <deque>
#include <atomic>

static PID_Base drive_pid_base = {{0.013, 0.0000, 0.02}, "Drive PID", DC_MOTOR_PID_MAX_OUTPUT, DC_MOTOR_PID_INTEGRAL_MAX, DC_MOTOR_PID_INTEGRAL_BEGIN};

static PID_Base rotate_pid_base = {{0.0, 0.0, 0.0}, "Rotate PID", 100, 0, DC_MOTOR_PID_INTEGRAL_BEGIN}; // kp = 0.1

class TaskManager
{
public:
    
    TaskManager();
    ~TaskManager();

    void run_tasks();

private: // Functions
    void move_to(std::vector<double> pose, double stop_threshold, bool pure_pursuit, int flipout_timer, std::vector<double> speeds);
    void move_to_pure_pursuit(std::vector<std::vector<double>> points, std::vector<double> final_point, std::vector<double> speeds);
    void odo();

private: // Variables
<<<<<<< HEAD

    std::unique_ptr<StepperMotor>   m_stepper;
    std::unique_ptr<DCMotors>     m_dc_motors;
=======
    std::unique_ptr<StepperMotor> m_stepper;
    std::unique_ptr<DCMotors> m_dc_motors;
    PID m_drive_pid = PID(drive_pid_base);
    PID m_rotate_pid = PID(rotate_pid_base);
    std::unique_ptr<Encoder> m_encoder;

    std::mutex m_position_mutex;
    point position;
    std::thread odo_thread;
    bool odo_running = true;
    double offset_back = 7.28;
    double offset_middle = 7.28;
    double wheel_circumference = 2.75 * M_PI;
>>>>>>> Final Code
};