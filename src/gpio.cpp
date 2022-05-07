#include "gpio.h"

void GPIO::gpioSetMode(unsigned gpio, unsigned mode)
{
    int reg, shift;

    reg = gpio / 10;
    shift = (gpio % 10) * 3;

    gpioReg[reg] = (gpioReg[reg] & ~(7 << shift)) | (mode << shift);
}

int GPIO::gpioGetMode(unsigned gpio)
{
    int reg, shift;

    reg = gpio / 10;
    shift = (gpio % 10) * 3;

    return (*(gpioReg + reg) >> shift) & 7;
}

void GPIO::gpioSetPullUpDown(unsigned gpio, unsigned pud)
{
    int shift = (gpio & 0xf) << 1;
    uint32_t bits;
    uint32_t pull;

    if (pi_is_2711)
    {
        switch (pud)
        {
        case PI_PUD_OFF:
            pull = 0;
            break;
        case PI_PUD_UP:
            pull = 1;
            break;
        case PI_PUD_DOWN:
            pull = 2;
            break;
        }

        bits = *(gpioReg + GPPUPPDN0 + (gpio >> 4));
        bits &= ~(3 << shift);
        bits |= (pull << shift);
        *(gpioReg + GPPUPPDN0 + (gpio >> 4)) = bits;
    }
    else
    {
        *(gpioReg + GPPUD) = pud;

        usleep(20);

        *(gpioReg + GPPUDCLK0 + PI_BANK) = PI_BIT;

        usleep(20);

        *(gpioReg + GPPUD) = 0;

        *(gpioReg + GPPUDCLK0 + PI_BANK) = 0;
    }
}

int GPIO::gpioRead(unsigned gpio)
{
    if ((*(gpioReg + GPLEV0 + PI_BANK) & PI_BIT) != 0)
        return 1;
    else
        return 0;
}

void GPIO::gpioWrite(unsigned gpio, unsigned level)
{
    if (level == 0)
        *(gpioReg + GPCLR0 + PI_BANK) = PI_BIT;
    else
        *(gpioReg + GPSET0 + PI_BANK) = PI_BIT;
}

void GPIO::gpioTrigger(unsigned gpio, unsigned pulseLen, unsigned level)
{
    if (level == 0)
        *(gpioReg + GPCLR0 + PI_BANK) = PI_BIT;
    else
        *(gpioReg + GPSET0 + PI_BANK) = PI_BIT;

    usleep(pulseLen);

    if (level != 0)
        *(gpioReg + GPCLR0 + PI_BANK) = PI_BIT;
    else
        *(gpioReg + GPSET0 + PI_BANK) = PI_BIT;
}

/* Bit (1<<x) will be set if gpio x is high. */

uint32_t GPIO::gpioReadBank1(void) { return (*(gpioReg + GPLEV0)); }
uint32_t GPIO::gpioReadBank2(void) { return (*(gpioReg + GPLEV1)); }

/* To clear gpio x bit or in (1<<x). */

void GPIO::gpioClearBank1(uint32_t bits) { *(gpioReg + GPCLR0) = bits; }
void GPIO::gpioClearBank2(uint32_t bits) { *(gpioReg + GPCLR1) = bits; }

/* To set gpio x bit or in (1<<x). */

void GPIO::gpioSetBank1(uint32_t bits) { *(gpioReg + GPSET0) = bits; }
void GPIO::gpioSetBank2(uint32_t bits) { *(gpioReg + GPSET1) = bits; }

unsigned GPIO::gpioHardwareRevision()
{
    static unsigned rev = 0;

    FILE *filp;
    char buf[512];
    char term;
    int chars = 4; /* number of chars in revision string */

    filp = fopen("/proc/cpuinfo", "r");

    if (filp != NULL)
    {
        while (fgets(buf, sizeof(buf), filp) != NULL)
        {
            if (!strncasecmp("revision", buf, 8))
            {
                if (sscanf(buf + strlen(buf) - (chars + 1),
                           "%x%c", &rev, &term) == 2)
                {
                    if (term != '\n')
                        rev = 0;
                    else
                        rev &= 0xFFFFFF; /* mask out warranty bit */
                }
            }
        }

        fclose(filp);
    }

    if (filp = fopen("/proc/device-tree/soc/ranges", "rb"))
    {
        if (fread(buf, 1, sizeof(buf), filp) >= 8)
        {
            piPeriphBase = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];
            if (!piPeriphBase)
                piPeriphBase = buf[8] << 24 | buf[9] << 16 | buf[10] << 8 | buf[11];

            if (piPeriphBase == 0xFE00000)
                pi_is_2711 = 1;
        }
        fclose(filp);
    }

    return rev;
}

/* Returns the number of microseconds after system boot. Wraps around
   after 1 hour 11 minutes 35 seconds.
*/

uint32_t GPIO::gpioTick(void) { return systReg[SYST_CLO]; }

/* Map in registers. */

static uint32_t *GPIO::initMapMem(int fd, uint32_t addr, uint32_t len)
{
    return (uint32_t *)mmap(0, len,
                            PROT_READ | PROT_WRITE | PROT_EXEC,
                            MAP_SHARED | MAP_LOCKED,
                            fd, addr);
}

int GPIO::gpioInitialise(void)
{
    int fd;

    gpioHardwareRevision(); /* sets rev and peripherals base address */

    fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (fd < 0)
    {
        fprintf(stderr,
                "This program needs root privileges.  Try using sudo\n");
        return -1;
    }

    gpioReg = initMapMem(fd, GPIO_BASE, GPIO_LEN);
    systReg = initMapMem(fd, SYST_BASE, SYST_LEN);
    bscsReg = initMapMem(fd, BSCS_BASE, BSCS_LEN);

    close(fd);

    if ((gpioReg == MAP_FAILED) ||
        (systReg == MAP_FAILED) ||
        (bscsReg == MAP_FAILED))
    {
        fprintf(stderr,
                "Bad, mmap failed\n");
        return -1;
    }
    return 0;
}