#include "encoder.h"
#include <string>

using namespace GPIO;

// #define DEBUG

Encoder::Encoder()
{
    gpio_set_function(DC_MOTOR_ENCODER_LEFT_A,  PI_FUNCTION::INPUT); // ENC 1A
    gpio_set_function(DC_MOTOR_ENCODER_LEFT_B,  PI_FUNCTION::INPUT); // ENC 1B
    gpio_set_function(DC_MOTOR_ENCODER_RIGHT_A, PI_FUNCTION::INPUT); // ENC 2A
    gpio_set_function(DC_MOTOR_ENCODER_RIGHT_B, PI_FUNCTION::INPUT); // ENC 2B

    m_position_left = 0;
    m_position_right = 0;

    m_encoder_active = true;
    m_measure_thread = std::thread(&Encoder::measure_encoder, this);

    Log::log_info("Encoder::Encoder - Encoders Created");
}

Encoder::~Encoder()
{
    m_encoder_active = false;
    m_measure_thread.join();
    Log::log_info("Encoder::Encoder - Encoders Destroyed");
}

int32_t Encoder::get_left_position()
{
    return m_position_left;
}

int32_t Encoder::get_right_position()
{
    return m_position_right;
}

void Encoder::measure_encoder()
{
    bool current_state_left, last_state_left = gpio_read(DC_MOTOR_ENCODER_LEFT_A);
    bool current_state_right, last_state_right = gpio_read(DC_MOTOR_ENCODER_RIGHT_A);

    while (m_encoder_active)
    {
        current_state_left = gpio_read(DC_MOTOR_ENCODER_LEFT_A);
        current_state_right = gpio_read(DC_MOTOR_ENCODER_RIGHT_A);

        if (current_state_left != last_state_left) // branchless?
        {
            m_position_left += ((gpio_read(DC_MOTOR_ENCODER_LEFT_B) != current_state_left) * 2) - 1;
            #if defined(DEBUG)
                Log::log_info("Encoder::measure_encoder - Encoder left position : " + std::to_string(m_position_left));
            #endif
        }

        if (current_state_right != last_state_right)
        {
            m_position_right += ((gpio_read(DC_MOTOR_ENCODER_RIGHT_B) != current_state_right) * 2) - 1;
            #if defined(DEBUG)
                Log::log_info("Encoder::measure_encoder - Encoder right position : " + std::to_string(m_position_right));
            #endif
        }

        last_state_left = current_state_left;
        last_state_right = current_state_right;
    }
}
