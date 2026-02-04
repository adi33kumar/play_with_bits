#pragma once
#include "peripherals/base.h"
#include "common.h"

struct GpioPinData
{
    reg32 reserved;
    reg32 data[2];
};

struct GpioRegisters
{
    reg32 function_select[6];
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData pin_level;
    struct GpioPinData event_detect_status;
    struct GpioPinData rising_edge_detect_enable;
    struct GpioPinData falling_edge_detect_enable;
    struct GpioPinData high_detect_enable;
    struct GpioPinData low_detect_enable;
    struct GpioPinData async_rising_edge_detect_enable;
    struct GpioPinData async_falling_edge_detect_enable;
    reg32 reserved;
    reg32 pupd_enable;
    reg32 pupd_enable_clock[2];
};
#define REGS_GPIO ((struct GpioRegisters*)(PBASE + 0x200000))