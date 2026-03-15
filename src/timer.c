#include "timer.h"
#include "printf.h"

uint32_t curr_val = 0;
const uint32_t interval = 1000000; // 1 second
void timer_init()
{
    curr_val = REGS_TIMER->counter_low;
    printf("Current timer value: %d\n", curr_val);
    curr_val += interval;
    REGS_TIMER->compare[1] = curr_val;
    curr_val = REGS_TIMER->counter_low;
    printf("Timer initialized, next interrupt at: %d\n", curr_val);

}

void timer_handler()
{
    // Clear the timer interrupt by writing to the control register
    // REGS_TIMER->control_status = 0;
    printf("Timer interrupt occurred!\n");
    // Schedule the next timer interrupt
    curr_val += interval;
    REGS_TIMER->compare[1] = curr_val;
    REGS_TIMER->control_status |= 2;
    printf("Timer 1 received\n");
    // Perform any additional actions needed on timer interrupt
}