#pragma once

#include "log.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

namespace GPIO
{
    enum class PI_FUNCTION : uint8_t
    {
        INPUT = 0b000,
        OUTPUT = 0b001,
        ALT_0 = 0b100,
        ALT_1 = 0b101,
        ALT_2 = 0b110,
        ALT_3 = 0b111,
        ALT_4 = 0b011,
        ALT_5 = 0b010
    };

    enum class PI_OUTPUT : bool
    {
        LOW = false,
        HIGH = true
    };

    // Page 66 - https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
    //
    // Register access is as follows:
    // Set   -  7 | HEX | 0x1C / 0x04 = 0x07 | DEC |  28 / 4 =  7
    // Clear - 10 | HEX | 0x28 / 0x04 = 0x0A | DEC |  40 / 4 = 10
    //
    // Where the code looks like:
    // gpio_reg[REG] = NEW_REG_VALUE
    //
    // Most '1' registers are not needed as the following formula can be used (e.g. SET1, CLR0)
    // REG1 = REG0 + 1 (expanded to)
    // REG1 = REG0 + (pin / 32)
    // 32 for the 32 bits in each register
    enum class PI_GPIO_REGISTERS : uint8_t
    {
        FSE0 = 0x00, // Function Select
        FSE1 = 0x01,
        FSE2 = 0x02,
        FSE3 = 0x03,
        FSE4 = 0x04,
        FSE5 = 0x05,
        /* RESERVED             = 0x06 */
        SET0 = 0x07, // Set output
        SET1 = 0x08,
        /* RESERVED             = 0x09 */
        CLR0 = 0x0A, // Clear output
        CLR1 = 0x0B,
        /* RESERVED             = 0x0C */
        LEV0 = 0x0D, // input Level
        LEV1 = 0x0E,
        /* RESERVED             = 0x0F */
        EDS0 = 0x10, // Event Detect Status
        EDS1 = 0x11,
        /* RESERVED             = 0x12 */
        RED0 = 0x13, // Rising Edge Detect
        RED1 = 0x14,
        /* RESERVED             = 0x15 */
        FED0 = 0x16, // Failing Edge Detect
        FED1 = 0x17,
        /* RESERVED             = 0x18 */
        HDE0 = 0x19, // High Detect
        HDE1 = 0x1A,
        /* RESERVED             = 0x1B */
        LDE0 = 0x1C, // Low Detect
        LDE1 = 0x1D,
        /* RESERVED             = 0x1E */
        ARE0 = 0x1F, // Async Rising Edge detect
        ARE1 = 0x20,
        /* RESERVED             = 0x21 */
        AFE0 = 0x22, // Async Failing Edge detect
        AFE1 = 0x23,
        /* RESERVED             = 0x24 - 0x38 */
        PUP0 = 0x39, // Pull Up / Pull down
        PUP1 = 0x3A,
        PUP2 = 0x3B,
        PUP3 = 0x3C
    };

    // Page 129 - https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
    enum class PI_PWM_REGISTERS : uint8_t
    {
        CTL = 0x00, // Control
        STA = 0x01, // Status
        DMA = 0x02, // DMA configuration
        /* RESERVED            = 0x03 */
        RNG1 = 0x04, // channel 1 Range
        DAT1 = 0x05, // channel 1 Data
        FIF = 0x06,  // FIFO Input
        /* RESERVED            = 0x07 */
        RNG2 = 0x08, // channel 2 Range
        DAT2 = 0x09  // channel 2 Data
    };

    void gpio_init();
    void gpio_set_function(uint8_t pin, PI_FUNCTION function);
    void gpio_write(uint8_t pin, PI_OUTPUT output);
    bool gpio_read(uint8_t pin);
    uint32_t sys_tick();
    void pwm_start(uint8_t pin);
    void pwm_stop();
    static void pwm_run();
    void pwm_write(uint32_t period_us, uint32_t duty_period_us);

    static std::thread pwm_thread;
    static std::atomic<uint32_t> pwm_period;
    static std::atomic<uint32_t> pwm_duty_period;
    static bool pwm_state;
    static uint8_t pwm_pin;

    static constexpr uint32_t gpio_base_address = 0xFE200000;
    static constexpr uint32_t gpio_len          = 0x000000F4;
    static constexpr uint32_t pwm_base_address  = 0xFE20C000; // PWM 0
    static constexpr uint32_t pwm_len           = 0x00000028;
    static constexpr uint32_t sys_base_address  = 0xFE003000;
    static constexpr uint32_t sys_len           = 0x0000001C;

    static volatile uint32_t *gpio_memory;
    static volatile uint32_t *pwm_memory;
    static volatile uint32_t *sys_memory;

}; //namespace GPIO