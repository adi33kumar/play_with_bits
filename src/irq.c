
#include "irq.h"

void irq_init()
{
    REGS_IRQ->enable1 = 1 << 1; // Enable timer interrupt
}

