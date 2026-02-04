#include "mini_uart.h"
#include "peripherals/aux.h"
#include "gpio.h"

void uart_init()
{
    // Initialize mini UART
    gpio_pin_set_func(14, 2); // TXD0
    gpio_pin_set_func(15, 2); // RXD0
    gpio_pin_enable(14);
    gpio_pin_enable(15);

    REGS_AUX->enables = 1; // Enable mini UART
    REGS_AUX->mu_cntl = 0; // Disable transmitter and receiver during configuration
    REGS_AUX->mu_lcr = 3; // 8-bit mode
    REGS_AUX->mu_mcr = 0; // RTS line disabled
    REGS_AUX->mu_ier = 0; // Disable interrupts
    REGS_AUX->mu_baud = 270; // Set baud rate to  115200 assuming 250Mhz system clock
    REGS_AUX->mu_cntl = 3; // Enable transmitter and receiver

}

void uart_send(char c)
{
    // Wait until the transmitter is empty
    while (!(REGS_AUX->mu_lsr & 0x20));
    REGS_AUX->mu_io = c;
}

char uart_recv()
{
    // Wait until data is available
    while (!(REGS_AUX->mu_lsr & 0x01));
    return REGS_AUX->mu_io & 0xFF;
}