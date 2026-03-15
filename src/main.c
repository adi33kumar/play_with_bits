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
// To demonstrate priority inversion.
SemaphoreHandle_t priorityInversionSem;;

void LowPriorityTask(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake( priorityInversionSem, portMAX_DELAY ) == pdTRUE) 
        {
            printf("Hello from Low Priority Task!\n");
            vTaskDelay(pdMS_TO_TICKS(1000)); // Simulate long processing time
            xSemaphoreGive( priorityInversionSem );
        }
        
    }
}

void MediumPriorityTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(100)); // Ensure this task runs after Low Priority Task has taken the semaphore
    while (1) {
        printf("Hello from Medium Priority Task!\n");
    }
}

void HighPriorityTask(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake( priorityInversionSem, portMAX_DELAY ) == pdTRUE) 
        {
            printf("Hello from High Priority Task!\n");
            xSemaphoreGive( priorityInversionSem );
            vTaskDelay(pdMS_TO_TICKS(200)); // Simulate some processing time
        }
        
    }
}



void kernel_main()
{
    uart_init();
    init_printf(0, putc);
    // initialize_performance_monitors();
    // uart_send('H');
    // uart_send('e');
    // uart_send('l');
    // uart_send('l');
    // uart_send('o');
    // uart_send(',');
    // uart_send(' ');
    // uart_send('W');
    // uart_send('o');
    // uart_send('r');
    // uart_send('l');
    // uart_send('d');
    // uart_send('!');
    // uart_send('\n');  
    // uint64_t start = read_pmccntr();
    // printf("This is a test of the printf function: %d, %s, %x\n", 42, "hello", 255);
    // uint64_t end = read_pmccntr();
    // printf("Printf took %d cycles\n", (uint32_t)(end - start));
    // irq_enable();
    // irq_init();
    // To initialize System Timer not ARM Timer.
    // timer_init();
    TaskHandle_t task1;  // Highest Priority Task
    TaskHandle_t task2;     // Medium Priority Task
    TaskHandle_t task3;     // Low Priority Task
    // Get the handle for binary Semaphore
    // priorityInversionSem = xSemaphoreCreateBinary();
    priorityInversionSem = xSemaphoreCreateMutex(); // Using a mutex to handle priority inversion
    xSemaphoreGive(priorityInversionSem); // Initially give the semaphore so that Low Priority Task can take it first.
    // Create task with different priorities
    xTaskCreate( LowPriorityTask, "Low Priority Task", 1024, NULL, 1, &task1 ); // Low Priority Task
    xTaskCreate( MediumPriorityTask, "Medium Priority Task", 1024, NULL, 2, &task2 ); // Medium Priority Task
    xTaskCreate( HighPriorityTask, "High Priority Task", 1024, NULL, 3, &task3 ); // High Priority Task

    vTaskStartScheduler(); // Start the FreeRTOS scheduler
    printf("Scheduler started\n");
    while (1) {
        /* Should never reach here. */
    }
}