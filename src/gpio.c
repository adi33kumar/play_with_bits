#include "peripherals/gpio.h"
#include "utils.h"
#include "common.h"
void gpio_pin_set_func(u8 pin, u8 func)
{
    u8 reg_index = pin / 10;
    int bit_offset = (pin *3) % 30;
    // read the register values
    reg32 current_value = REGS_GPIO->function_select[reg_index];
    // clear the 3 bits corresponding to the pin
    current_value &= ~(0b111 << bit_offset);
    // set the new function
    current_value |= (func << bit_offset);
    // write back the modified value
    REGS_GPIO->function_select[reg_index] = current_value;
}

void gpio_pin_enable(u8 pin)
{
    REGS_GPIO->pupd_enable = 0; // disable pull up/down
    delay(150);
    // clock the control signal into gpio pins
    REGS_GPIO->pupd_enable_clock[pin / 32] = (1 << (pin % 32));
    delay(150);
    
    REGS_GPIO->pupd_enable = 0; // remove the control signal
    REGS_GPIO->pupd_enable_clock[pin / 32] = 0; // remove the clock
}
