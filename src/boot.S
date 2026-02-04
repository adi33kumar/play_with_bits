
#include "mm.h"
#include "sysreg.h"

.section ".text.boot"

.globl _start
_start:
    // lower 8 bit shows affinity 0 bits
    mrs x0, mpidr_el1;
    and x0, x0, #0xff;
    cbz x0, master
    b proc_hang

master:
    // disable the MMU
    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

#if 0
    // Hypervisor configuration register
    ldr x0, =HCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    // Saved program status register
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0
#endif

#if 1 // el2 settings
    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0

    adr x0, el1_entry
    msr elr_el2, x0
#endif

    eret

el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    mov sp, #LOW_MEMORY
    bl kernel_main
    b proc_hang

proc_hang:
    wfe
    b proc_hang