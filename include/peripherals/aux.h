#pragma once
#include "common.h"

#include "peripherals/base.h"

struct AuxRegisters
{
    reg32 irq_enable;
    reg32 enables;
    reg32 reserved1[14];
    reg32 mu_io;
    reg32 mu_ier;
    reg32 mu_iir;
    reg32 mu_lcr;
    reg32 mu_mcr;
    reg32 mu_lsr;
    reg32 mu_msr;
    reg32 mu_scratch;
    reg32 mu_cntl;
    reg32 mu_stat;
    reg32 mu_baud;
};

#define REGS_AUX ((struct AuxRegisters*)(PBASE + 0x215000))