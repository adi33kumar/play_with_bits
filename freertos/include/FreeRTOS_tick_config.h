#define CORE_IRQ_SOURCE ((volatile uint32_t *)(0x40000060))
#define CORE_TIMER_IRQCNTL ((volatile uint32_t *)(0x40000040))
void print_cntv_reg();
void interrupt_stats();
void vDefaultExpHandler(uint64_t type);