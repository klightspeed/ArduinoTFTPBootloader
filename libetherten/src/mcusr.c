#include "libetherten/mcusr.h"
#include <stdint.h>

uint8_t mcusr_mirror __attribute__((section(".noinit")));

void __attribute__((naked,section(".init1"))) __save_mcusr(void) {
    asm volatile ("sts mcusr_mirror, r2");
}

