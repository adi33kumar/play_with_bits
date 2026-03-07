#include <stdint.h>
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_tick_config.h"

static uint32_t timer_cntfrq = 0;
static uint32_t timer_tick = 0;
static uint32_t g_cntv_ctl = 1;

void enable_cntv(void)
{
	uint32_t cntv_ctl;
	cntv_ctl = g_cntv_ctl;
	asm volatile ("msr cntv_ctl_el0, %0" :: "r" (cntv_ctl));
}
/*-----------------------------------------------------------*/

void write_cntv_tval(uint32_t val)
{
	asm volatile ("msr cntv_tval_el0, %0" :: "r" (val));
	return;
}

uint32_t read_cntfrq(void)
{
	uint32_t val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
	return val;
}

#define CORE_TIMER_IRQCNTL ((volatile uint32_t *)(0x40000040))


void vMainAssertCalled(const char * pcFile, unsigned long ulLine) {
    printf("Assert called in file %s at line %d\n", pcFile, ulLine);
    while(1);
}

void init_timer(void)
{
	timer_cntfrq = timer_tick = read_cntfrq();
	printf("cntfrq=%d\n", timer_cntfrq);
	write_cntv_tval(timer_cntfrq);    // clear cntv interrupt and set next 1 msec timer.
	return;
}

void timer_set_tick_rate_hz(uint32_t rate)
{
	timer_tick = timer_cntfrq / rate ;
	write_cntv_tval(timer_tick);
    printf("timer_tick=%d\n", timer_tick);
}

void* memcpy(void* dest, const void* src, uint32_t n) {
    char* d = (char*) dest;
    const char* s = (const char*) src;
    for (uint32_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
   application must provide an implementation of vApplicationGetTimerTaskMemory()
   to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
       function then they must be declared static - otherwise they will be allocated on
       the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
       task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
       Note that, as the array is necessarily of type StackType_t,
      configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void interrupt_stats()
{
    printf("Interrupt stats\n");
    printf("CORE0_TIMER_IRQCNTL: %08X\n", *CORE_TIMER_IRQCNTL);
    printf("CORE0_IRQ_SOURCE: %08X\n", *CORE_IRQ_SOURCE);
}

void print_cntv_reg()
{
	
	uint64_t cntv_cval, cntv_ctl, cntv_tval, cntvct;
	asm volatile ("mrs %0, cntv_ctl_el0" : "=r" (cntv_ctl));
	asm volatile ("mrs %0, CNTV_CVAL_EL0" : "=r" (cntv_cval));
	asm volatile ("mrs %0, CNTVCT_EL0" : "=r" (cntvct));
	asm volatile ("mrs %0, CNTV_TVAL_EL0" : "=r" (cntv_tval));
	printf("cntv_ctl=%08X\n", (uint32_t)cntv_ctl);
	printf("cntv_cval=%08X\n", (uint32_t)cntv_cval);
	printf("cntvct=%08X\n", (uint32_t)cntvct);
	printf("cntv_tval=%08X\n", (uint32_t)cntv_tval);

}

void vConfigureTickInterrupt( void )
{
	/* init timer device. */
	init_timer();

	/* set tick rate. */
	timer_set_tick_rate_hz(configTICK_RATE_HZ);
    
	/* timer interrupt routing. */
	*CORE_TIMER_IRQCNTL = 1 << 3; /* nCNTVIRQ routing to CORE0.*/

    printf("after timer interrupt routing\n");
    interrupt_stats();
	/* start & enable interrupts in the timer. */
	enable_cntv();
    print_cntv_reg();
	// print_cntv_reg();
	// while(1);
}

void vApplicationIRQHandler( uint32_t ulCORE0_INT_SRC )
{
	uint32_t ulInterruptID;
	ulInterruptID = ulCORE0_INT_SRC & 0x0007FFFFUL;
	// if(gIrqCount < 2)
	// {
	// 	gIrqCount++;
	// 	printf("%1X\n", ulInterruptID);
	// }
	
	/* call handler function */
	if(ulInterruptID & (1 << 3))
	{
		/* Generic Timer */
		// if(gIrqCount%1000 == 0)
		// {
		// 	printf("%d\n", gIrqCount);
		// }
		// gIrqCount++;
		FreeRTOS_Tick_Handler();
	}
	// if(ulInterruptID & (1 << 8))
	// {
	// 	/* Peripherals */
	// 	irq_handler();
	// }
}

void vClearTickInterrupt( void )
{
	write_cntv_tval(timer_tick);    // clear cntv interrupt and set next timer.
	return;
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
   implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
   used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
       function then they must be declared static - otherwise they will be allocated on
       the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
       state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
       Note that, as the array is necessarily of type StackType_t,
       configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationTickHook( void )
{
}
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	printf("Stack overflow in task %s\n", pcTaskName);
	while(1);
}

void vApplicationIdleHook( void )
{
}

void *memset(void *str, int c, size_t n) {
    unsigned char *ptr = (unsigned char *)str;
    unsigned char value = (unsigned char)c;

    while (n--) {
        *ptr++ = value;
    }
    return str;
}

size_t strlen(const char *str) {
    const char *s = str;
    while (*s) {
        s++;
    }
    return s - str;
}

const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};

void show_invalid_entry_message(uint32_t type, uint64_t esr, uint64_t address) {
    printf("ERROR CAUGHT: %s - %d, ESR: %X, Address: %X\n", 
        entry_error_messages[type], type, esr, address);
}
uint32_t get_el();
void vDefaultExpHandler(uint64_t type)
{
	printf("Exception type=%d\n", type);
	printf("\nException Level: %d\n", get_el());
	uint64_t far_el1;
    __asm__ volatile ("mrs %0, far_el1" : "=r" (far_el1));
    printf("Fault Address Register (FAR_EL1): %d\n", (uint32_t)far_el1);
	uint64_t esr_el1;
    // 1. Read the Exception Syndrome Register (ESR_EL1)
    //    Use inline assembly (GCC/Clang syntax) to read the register
    __asm__ volatile ("mrs %0, esr_el1" : "=r" (esr_el1));
    printf("ESR_EL1 Value: %d\n", (uint32_t)esr_el1);
	uint64_t elr_el1;
    // 1. Read the Exception Syndrome Register (ESR_EL1)
    //    Use inline assembly (GCC/Clang syntax) to read the register
    __asm__ volatile ("mrs %0, elr_el1" : "=r" (elr_el1));
    printf("ELR_EL1 Value: %d\n", (uint32_t)elr_el1);
	uint64_t spsr_el1;
    // 1. Read the Exception Syndrome Register (ESR_EL1)
    //    Use inline assembly (GCC/Clang syntax) to read the register
    __asm__ volatile ("mrs %0, spsr_el1" : "=r" (spsr_el1));
    printf("SPSR_EL1 Value: %d\n", (uint32_t)spsr_el1);
	while(1);
}

void initialize_performance_monitors()
{
    uint64_t val;
	/* Disable cycle counter overflow interrupt */
	asm volatile("msr pmintenset_el1, %0" : : "r" ((uint64_t)(0 << 31)));
	/* Enable cycle counter */
	asm volatile("msr pmcntenset_el0, %0" :: "r" (1 << 31));
	/* Enable user-mode access to cycle counters. */
	asm volatile("msr pmuserenr_el0, %0" :: "r" ((1 << 0) | (1 << 2)));
	/* Clear cycle counter and start */
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	val |= ((1 << 0) | (1 << 2));
	asm volatile("isb");
	asm volatile("msr pmcr_el0, %0" :: "r" (val));
	val = (1 << 27);
	asm volatile("msr pmccfiltr_el0, %0" :: "r" (val));

}

inline uint64_t read_pmccntr(void)
{
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return val;
}