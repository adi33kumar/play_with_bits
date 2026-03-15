
#include <stdint.h>

struct timer_regs 
{
    uint32_t control_status;
    uint32_t counter_low;
    uint32_t counter_high;
    uint32_t compare[4];
};
void timer_init();
void timer_handler();

#define REGS_TIMER ((volatile struct timer_regs *)(0x3f000000 + 0x3000))