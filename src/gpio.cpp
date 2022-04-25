#include "gpio.h"
#include <sys/ioctl.h>
#include <string.h>

#define PWM_FREQ        100000
#define PWM_RANGE       20000
#define CLOCK_KHZ       375000
#define CLK_PASSWD      0x5A000000

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
    pwm_memory  = static_cast<volatile uint32_t *>(mmap(0,  pwm_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd,  pwm_base_address));
    dma_memory  = static_cast<volatile uint32_t *>(mmap(0,  dma_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd,  dma_base_address));
    clk_memory  = static_cast<volatile uint32_t *>(mmap(0,  clk_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd,  clk_base_address));
    sys_memory  = static_cast<volatile uint32_t *>(mmap(0,  sys_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd,  sys_base_address));

    // Virtual Memory
    vir_dma_memory = static_cast<volatile uint32_t *>(mmap(0, vir_dma_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd, vir_dma_base_address));

    close(fd);
}

void GPIO::gpio_set_function(uint8_t pin, PI_FUNCTION function)
{
    uint8_t reg = pin / 10; // Will be equal to the FSEL register for the given pin
    uint8_t shift = (pin % 10) * 3;

    // Page 67 - https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
    // ~(7 << shift)       - Create a mask of 000 and shift it to the pin index 
    // gpio_reg[reg] & ... - Bitwise AND the register with masked function value
    // (function << shift) - shift the new function to the pin index
    // ... | ...           - Bitwise or the new function value and the register with mask 
    gpio_memory[reg] = (gpio_memory[reg] & ~(7 << shift)) | ((int)function << shift);
}

void GPIO::gpio_write(uint8_t pin, bool output)
{
    // If output is high (true) we want to set the bit, if low we want to clear the bit
    // We then add pin / 32 to the reg to access SET1 and CLR1 if the pin is > 32
    // Finally, we get the pin itself by using pin mod 32, and shifting a 1 to that index
    uint8_t reg = (int)PI_GPIO_REGISTERS::SET0 + (!output * 3);
    gpio_memory[reg + (pin / 32)] = 1 << (pin % 32);
}

bool GPIO::gpio_read(uint8_t pin)
{
    // This is the same logic as writing the pin, but in reverse
    uint8_t shift = (pin % 32);
    return (gpio_memory[(int)PI_GPIO_REGISTERS::LEV0 + (pin / 32)] & (1 << shift)) >> shift;
}

uint32_t GPIO::sys_tick()
{
    // Time in us - micro seconds 1e-6
    return sys_memory[1]; // Clock is register 1
}

void GPIO::pwm_init()
{
    dma_memory[(int)PI_DMA_REGISTERS::CS_5]    = 1 << 31; // Reset DMA 5 
    dma_memory[(int)PI_DMA_REGISTERS::ENABLE] |= 1 << 5;  // Enable DMA 5 
    
    DMA_CB *cbs = (DMA_CB*)vir_dma_memory;
    uint32_t *pindata = (uint32_t *)(cbs + 4);
    uint32_t *pwmdata = pindata + 1;

    memset(cbs, 0, sizeof(DMA_CB) * 4);

    // Transfers are triggered by PWM request
    cbs[0].ti = cbs[1].ti = cbs[2].ti = cbs[3].ti = (1 << 6) | (5 << 16);

    // Control block 0 and 2: clear & set LED pin, 4-byte transfer
    cbs[0].srce_ad = cbs[2].srce_ad = (uint32_t)((uint64_t)(pindata) - (uint64_t)vir_dma_memory + bus_dma_base_address);
    cbs[0].dest_ad = gpio_memory[(int)PI_GPIO_REGISTERS::CLR0];
    cbs[2].dest_ad = gpio_memory[(int)PI_GPIO_REGISTERS::SET0];
    cbs[0].tfr_len = cbs[2].tfr_len = 4;

    // Control block 1 and 3: update PWM FIFO (to clear DMA request)
    cbs[1].srce_ad = cbs[3].srce_ad = (uint32_t)((uint64_t)(pwmdata) - (uint64_t)vir_dma_memory + bus_dma_base_address);
    cbs[1].dest_ad = cbs[3].dest_ad = pwm_memory[(int)PI_PWM_REGISTERS::FIF];
    cbs[1].tfr_len = cbs[3].tfr_len = 4;

    // Link control blocks 0 to 3 in endless loop
    for (int n = 0; n < 4; n++)
        cbs[n].next_cb = (uint32_t)((uint64_t)(&cbs[(n + 1) % 4]) - (uint64_t)vir_dma_memory + bus_dma_base_address); 

    dma_memory[(int)PI_DMA_REGISTERS::CONBLOK_5] = (uint32_t)((uint64_t)(&cbs[0]) - (uint64_t)vir_dma_memory + bus_dma_base_address); // Give first control block
    dma_memory[(int)PI_DMA_REGISTERS::CS_5] = 2; // Clear end flag
    dma_memory[(int)PI_DMA_REGISTERS::DEBUG_5] = 7; // Clear error bits
    dma_memory[(int)PI_DMA_REGISTERS::CS_5] = 1; // Start DMA

    // Disable PWM to change settings
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0;

    int divi= (CLOCK_KHZ * 1000) / PWM_FREQ;

    clk_memory[0x28] = CLK_PASSWD | (1 << 5);
    while (clk_memory[0x28] & (1 << 7)) ;
    clk_memory[0x29] = CLK_PASSWD | (divi << 12);
    clk_memory[0x28] = CLK_PASSWD | 6 | (1 << 4);
    while ((clk_memory[0x28] & (1 << 7)) == 0) ;

    pwm_memory[(int)PI_PWM_REGISTERS::RNG1] = PWM_RANGE;
    pwm_memory[(int)PI_PWM_REGISTERS::FIF] =  (PWM_RANGE / 2);
    pwm_memory[(int)PI_PWM_REGISTERS::DMA] = (1 << 31) | 1; // data threshold of 1 and enable
}

void GPIO::pwm_start(PI_PWM_CHANNEL channel)
{
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0x40;  // Use FIFO and Enable
    usleep(10);
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0;  // Use FIFO and Enable
    usleep(10);
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0x2121;  // Use FIFO and Enable
    usleep(10);
    std::cout << "End status : " << std::hex << pwm_memory[(int)PI_PWM_REGISTERS::STA] << std::endl;
}

void GPIO::pwm_stop()
{
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0; // stop pwm
    pwm_memory[(int)PI_PWM_REGISTERS::DMA] = 0x707;
}