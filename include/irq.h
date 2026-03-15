
#include <stdint.h>
struct irq_regs
{
    uint32_t pending_basic;
    uint32_t pending1;
    uint32_t pending2;
    uint32_t fiq_control;
    uint32_t enable1;
    uint32_t enable2;
    uint32_t enable_basic;
    uint32_t disable1;
    uint32_t disable2;
    uint32_t disable_basic;
};
void irq_init();
#define REGS_IRQ ((volatile struct irq_regs *)(0x3f000000 + 0xB200))