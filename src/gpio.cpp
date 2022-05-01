#include "gpio.h"
#include <sys/ioctl.h>
#include <string.h>

#define PWM_FREQ        375000
#define PWM_RANGE       20000
#define CLOCK_KHZ       375000
#define CLK_PASSWD      0x5A000000

void GPIO::gpio_init()
{
    VC_MSG msg;
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
    msg = {.tag=0x3000c, .blen=12, .dlen=12, .uints={0x1000, 0x1000, (1 << 2 + 1 << 4)}};
    uint32_t dma_mem_h = msg_mbox(mail_fd, &msg);
    msg = {.tag=0x3000d, .blen=4, .dlen=4, .uints={dma_mem_h}};
    bus_dma_memory = static_cast<volatile uint32_t *>((dma_mem_h ? (void*)msg_mbox(mail_fd, &msg) : 0));
    vir_dma_memory = static_cast<volatile uint32_t *>(mmap(0, vir_dma_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_LOCKED, fd, (uint32_t)((uint64_t)bus_dma_memory & ~0xC0000000)));

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
    
    DMA_TEST_DATA *dp = (DMA_TEST_DATA*)vir_dma_memory;
    DMA_TEST_DATA dma_data = { 
        .cbs = 
        {
          // TI      Srce addr          Dest addr        Len   Next CB
            {0x50040, MEM(&dp->pindata), gpio_memory[(int)PI_GPIO_REGISTERS::SET0], 4, 0, MEM(&dp->cbs[1]), 0},  // 0
            {0x50040, MEM(&dp->pwmdata), pwm_memory[(int)PI_PWM_REGISTERS::FIF]   , 4, 0, MEM(&dp->cbs[2]), 0},  // 1
            {0x50040, MEM(&dp->pindata), gpio_memory[(int)PI_GPIO_REGISTERS::SET0], 4, 0, MEM(&dp->cbs[3]), 0},  // 2
            {0x50040, MEM(&dp->pwmdata), pwm_memory[(int)PI_PWM_REGISTERS::FIF]   , 4, 0, MEM(&dp->cbs[0]), 0},  // 3
        },
        .pindata= 1 << 18,
        .pwmdata = PWM_RANGE / 2
    };
    memcpy(dp, &dma_data, sizeof(dma_data));    // Copy data into uncached memory
  
    // Disable PWM to change settings
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0;

    if (pwm_memory[(int)PI_PWM_REGISTERS::STA] & 0x100)
    {
        printf("PWM bus error\n");
        pwm_memory[(int)PI_PWM_REGISTERS::STA] = 0x100;
    }

    int divi= (CLOCK_KHZ * 1000) / PWM_FREQ;

    clk_memory[0x28] = CLK_PASSWD | (1 << 5);
    while (clk_memory[0x28] & (1 << 7)) ;
    clk_memory[0x29] = CLK_PASSWD | (divi << 12);
    clk_memory[0x28] = CLK_PASSWD | 6 | (1 << 4);
    while ((clk_memory[0x28] & (1 << 7)) == 0) ;

    pwm_memory[(int)PI_PWM_REGISTERS::RNG1] = PWM_RANGE;
    pwm_memory[(int)PI_PWM_REGISTERS::FIF] =  (PWM_RANGE / 2);
}

void GPIO::pwm_start()
{
    pwm_memory[(int)PI_PWM_REGISTERS::DMA] = (1 << 31) | 1; // data threshold of 1 and enable
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0x21;  // Use FIFO and Enable
}

void GPIO::pwm_stop()
{
    pwm_memory[(int)PI_PWM_REGISTERS::CTL] = 0; // stop pwm
    pwm_memory[(int)PI_PWM_REGISTERS::DMA] = 0x707;
}

uint32_t GPIO::msg_mbox(int fd, VC_MSG *msgp)
{
    for (uint32_t i = msgp->dlen / 4; i <= msgp->blen / 4; i += 4)
    {
        msgp->uints[i++] = 0;
    }
    msgp->len = (msgp->blen + 6) * 4;
    msgp->req = 0;
    ioctl(fd, _IOWR(100, 0, void *), msgp);
    return msgp->uints[0];
}