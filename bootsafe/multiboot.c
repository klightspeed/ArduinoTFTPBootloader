#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/fuse.h>
#include "boot-funcs.h"

extern uint8_t __stack[];

FUSES = {
    .low = (unsigned char)~0,
    .high = FUSE_BOOTSZ0 & FUSE_EESAVE & FUSE_SPIEN,
    .extended = FUSE_BODLEVEL0 & FUSE_BODLEVEL1,
};

void __attribute__((used,naked)) __init(uint32_t magic) {
    asm volatile ("eor r1, r1");
    SP = (uint16_t)(&__stack);

    uint8_t mcusr = MCUSR;

    if (mcusr & _BV(EXTRF)) {
        __mcusr_mirror = mcusr;
        asm volatile ("jmp __optiboot_start");
        __builtin_unreachable();
    }

    MCUSR = 0;
    wdt_reset();
    wdt_disable();

    if (__mcusr_mirror == 0 || (mcusr & _BV(PORF))) {
        __mcusr_mirror = mcusr;
    }

    asm volatile ("lds r2, __mcusr_mirror");

    if ((mcusr & _BV(PORF)) || magic == 0xc0decafe) {
        asm volatile ("jmp __bootloader_start");
        __builtin_unreachable();
    } else {
        __mcusr_mirror = 0;
        asm volatile ("jmp __application_start");
        __builtin_unreachable();
    }
}

