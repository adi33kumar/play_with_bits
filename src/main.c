#include "peripherals/aux.h"
#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_tick_config.h"
#include "semphr.h"
#include "timer.h"
#include "irq.h"


SemaphoreHandle_t xSemaphore;
void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }

    uart_send(c);
}
void Task1(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE) 
        {
            printf("Hello from Task 1!\n");
            xSemaphoreGive( xSemaphore );
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000 ms
    }
}

void Task2(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake( xSemaphore, portMAX_DELAY ) == pdTRUE) 
        {
            printf("Hello from Task 2!\n");
            xSemaphoreGive( xSemaphore );
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1000 ms
    }
}



void kernel_main()
{
    uart_init();
    init_printf(0, putc);
    initialize_performance_monitors();
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
    uint64_t start = read_pmccntr();
    printf("This is a test of the printf function: %d, %s, %x\n", 42, "hello", 255);
    uint64_t end = read_pmccntr();
    printf("Printf took %d cycles\n", (uint32_t)(end - start));
    irq_enable();
    irq_init();
    // To initialize System Timer not ARM Timer.
    timer_init();
    TaskHandle_t task1;
    TaskHandle_t task2;
    
    xSemaphore = xSemaphoreCreateBinary();
   if( xSemaphore == NULL )
 {
    printf("Failed to create semaphore\n");
    while(1);
 }
 else
 {
    printf("Semaphore created successfully\n");
 }
    xSemaphoreGive( xSemaphore );
    // BaseType_t ret = xTaskCreate( Task1, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task1 );
    // if(ret != pdPASS) {
    //     printf("Task creation failed\n");
    // }
    // BaseType_t ret2 = xTaskCreate( Task2, "Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task2 );
    // if(ret2 != pdPASS) {
    //     printf("Task2 creation failed\n");
    // }
    // else {
    //     printf("Task2 created successfully\n");
    // }
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