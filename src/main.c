#include "peripherals/aux.h"
#include "utils.h"
#include "mini_uart.h"

void kernel_main()
{
    uart_init();
    uart_send('H');
    uart_send('e');
    uart_send('l');
    uart_send('l');
    uart_send('o');
    uart_send(',');
    uart_send(' ');
    uart_send('W');
    uart_send('o');
    uart_send('r');
    uart_send('l');
    uart_send('d');
    uart_send('!');
    uart_send('\n');    

    // Kernel main function
    while (1)
    {
        char c = uart_recv(); // Receive a character
        uart_send(c);         // Echo the character back
    }
}