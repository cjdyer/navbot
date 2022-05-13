#include "gpio.h"
#include <sys/ioctl.h>
#include <string.h>

void GPIO::gpio_init()
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd < 0)
    {
        Log::log_info("GPIO::gpio_init - This program needs root privileges.  Try using sudo");
        return;
    }

    // Physical Memory
    gpio_memory = static_cast<volatile uint32_t *>(mmap(0, gpio_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd, gpio_base_address));
    sys_memory  = static_cast<volatile uint32_t *>(mmap(0,  sys_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd,  sys_base_address));

    close(fd);
}

void GPIO::gpio_set_function(uint8_t pin, PI_FUNCTION function)
{
    std::lock_guard<std::mutex> lk(m_pin_mutex);
    uint8_t reg = pin / 10; // Will be equal to the FSEL register for the given pin
    uint8_t shift = (pin % 10) * 3;

    // Page 67 - https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
    // ~(7 << shift)       - Create a mask of 000 and shift it to the pin index 
    // gpio_reg[reg] & ... - Bitwise AND the register with masked function value
    // (function << shift) - shift the new function to the pin index
    // ... | ...           - Bitwise or the new function value and the register with mask 
    gpio_memory[reg] = (gpio_memory[reg] & ~(7 << shift)) | ((int)function << shift);
}

void GPIO::gpio_write(uint8_t pin, PI_OUTPUT output)
{
    std::lock_guard<std::mutex> lk(m_pin_mutex);
    // If output is high (true) we want to set the bit, if low we want to clear the bit
    // We then add pin / 32 to the reg to access SET1 and CLR1 if the pin is > 32
    // Finally, we get the pin itself by using pin mod 32, and shifting a 1 to that index
    uint8_t reg = (int)PI_GPIO_REGISTERS::SET0 + (!((bool)output) * 3);
    gpio_memory[reg + (pin / 32)] = 1 << (pin % 32);
}

void GPIO::gpio_write(uint8_t pin, bool output)
{
    std::lock_guard<std::mutex> lk(m_pin_mutex);
    // If output is high (true) we want to set the bit, if low we want to clear the bit
    // We then add pin / 32 to the reg to access SET1 and CLR1 if the pin is > 32
    // Finally, we get the pin itself by using pin mod 32, and shifting a 1 to that index
    uint8_t reg = (int)PI_GPIO_REGISTERS::SET0 + (!(output) * 3);
    gpio_memory[reg + (pin / 32)] = 1 << (pin % 32);
}

bool GPIO::gpio_read(uint8_t pin)
{
    std::lock_guard<std::mutex> lk(m_pin_mutex);
    // This is the same logic as writing the pin, but in reverse
    uint8_t shift = (pin % 32);
    return (gpio_memory[(int)PI_GPIO_REGISTERS::LEV0 + (pin / 32)] & (1 << shift)) >> shift;
}

uint32_t GPIO::sys_tick()
{
    // Time in us - micro seconds 1e-6
    return sys_memory[1]; // Clock is register 1
}