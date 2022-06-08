#include "task_manager.h"
#include <string>

<<<<<<< HEAD
TaskManager::TaskManager() : 
    m_stepper(std::make_unique<StepperMotor>()),
    m_dc_motors(std::make_unique<DCMotors>())
{    
    Log::log_info("TaskManager::TaskManager - Started Task Manager");
=======
#define TO_RAD(n) n *M_PI / 180;
using std::placeholders::_1;

TaskManager::TaskManager() : m_stepper(std::make_unique<StepperMotor>()),
                             m_dc_motors(std::make_unique<DCMotors>()),
                             m_encoder(std::make_unique<Encoder>())
{
    Log::log_info("TaskManager::TaskManager - Started Task Manager");

    odo_thread = std::thread(&TaskManager::odo, this);
>>>>>>> Final Code
}

TaskManager::~TaskManager()
{
<<<<<<< HEAD
=======
    m_dc_motors->run_left_motor(0);
    m_dc_motors->run_right_motor(0);
    odo_running = false;
    usleep(600);
    odo_thread.join();
>>>>>>> Final Code
    Log::log_info("TaskManager::~TaskManager - Task Manager terminated gracefully...");
}

void TaskManager::run_tasks()
{
<<<<<<< HEAD
    m_dc_motors->disable();
    sleep(1);
=======
}

void TaskManager::move_to(std::vector<double> pose, double stop_threshold, bool pure_pursuit, int flipout_timer, std::vector<double> speeds)
{
    double new_y = pose[0];
    double new_x = pose[1];
    double heading = pose[2];

    std::deque<double> motion;

    double y_error = new_y - position.y;
    double x_error = -(new_x - position.x);
    int coefficient = 0;
    double last_x = position.x;
    double last_y = position.y;
    std::string powered = "intakes";

    int time = 0;

    double heading2 = (heading < 0) ? heading + 360 : heading - 360;
    if (pure_pursuit)
        heading = (abs(rotation - heading) < abs(rotation - heading2)) ? heading : heading2;
    double imu_error = -(rotation - heading);

    while (abs(y_error) > 10 || abs(x_error) > 10 || abs(imu_error) > 1)
    { 

        if ((int)motion.size() == 10)
            motion.pop_front();
        motion.push_back(abs(last_x - position.x) + abs(last_y - position.y));
        double sum = 0;
        for (int i = 0; i < motion.size(); i++)
            sum += motion[i];
        double motion_average = sum / 10;
        if (motion_average < stop_threshold && time > 100)
            break;

        last_x = position.x;
        last_y = position.y;

        double phi = TO_RAD(rotation);
        double power = m_drive_pid.calculate(y_error * std::cos(phi) + x_error * std::sin(phi)) * speeds[0];
        double strafe =  x_error * std::cos(phi) - y_error * std::sin(phi) * speeds[1];
        double turn = m_rotate_pid.calculate(imu_error) * speeds[2];

        m_dc_motors->run_left_motor(power - turn);
        m_dc_motors->run_right_motor(power + turn);
        m_stepper->go_to(strafe);

        imu_error = -(rotation - heading);
        y_error = new_y - position.y;
        x_error = -(new_x - position.x);
        if (pure_pursuit)
            return;
        usleep(5000);
        time += 5;
    }
}

void TaskManager::move_to_pure_pursuit(std::vector<std::vector<double>> points, std::vector<double> final_point, std::vector<double> speeds)
{
    point start, end, target;
    point cur = position;
    double heading;

    for (int index = 0; index < points.size() - 1; index++)
    {
        start = points[index];
        end = points[index + 1];

        while (purepursuit::distance(cur, end) > 300)
        {
            target = purepursuit::get_intersection(start, end, cur, 300);
            heading = purepursuit::get_degrees(target, cur);

            std::vector<double> pose{target.x, target.y, heading};
            move_to(pose, 0, true, 0, {2, 2, 2});
            cur = position;
        }
    }
    move_to(final_point, 3);
}

void TaskManager::odo()
{
    double last_x, last_y, last_phi, turn_offset_x, turn_offset_y;

    while (odo_running)
    {
        double cur_phi = TO_RAD(rotation);
        double dphi = cur_phi - last_phi;

        double cur_turn_offset_x = 360 * (offset_back * dphi) / wheel_circumference;
        double cur_turn_offset_y = 360 * (offset_middle * dphi) / wheel_circumference;

        turn_offset_x = turn_offset_x + cur_turn_offset_x;
        turn_offset_y = turn_offset_y + cur_turn_offset_y;

        double cur_y = ((m_encoder->get_left_position() - turn_offset_y) + (m_encoder->get_right_position() + turn_offset_y)) / 2;
        double cur_x = m_stepper->get_pos() - turn_offset_x;

        double dy = cur_y - last_y;
        double dx = cur_x - last_x;

        double global_dy = dy * std::cos(cur_phi) + dx * std::sin(cur_phi);
        double global_dx = dx * std::cos(cur_phi) - dy * std::sin(cur_phi);

        position = {position.x + global_dx, position.y + global_dy};

        last_y = cur_y;
        last_x = cur_x;
        last_phi = cur_phi;
    }
>>>>>>> Final Code
}