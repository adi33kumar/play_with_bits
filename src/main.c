#include "peripherals/aux.h"
#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_tick_config.h"

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}
void Task1(void *pvParameters) {
    while (1) {
        printf("Hello from Task 1!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000 ms
    }
}

void Task2(void *pvParameters) {
    while (1) {
        printf("Hello from Task 2!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000 ms
    }
}



void kernel_main()
{
    uart_init();
    init_printf(0, putc);
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
    printf("This is a test of the printf function: %d, %s, %x\n", 42, "hello", 255);
    TaskHandle_t task1;
    TaskHandle_t task2;
   
    BaseType_t ret = xTaskCreate( Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task1 );
    if(ret != pdPASS) {
        printf("Task creation failed\n");
    }
    BaseType_t ret2 = xTaskCreate( Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task2 );
    if(ret2 != pdPASS) {
        printf("Task2 creation failed\n");
    }
    else {
        printf("Task2 created successfully\n");
    }

    interrupt_stats();
    print_cntv_reg();
    // Kernel main function
    // while (1)
    // {
    //     char c = uart_recv(); // Receive a character
    //     uart_send(c);         // Echo the character back
    // }
    vTaskStartScheduler(); // Start the FreeRTOS scheduler
    printf("Scheduler started\n");
    while (1) {
        /* Should never reach here. */
    }
}