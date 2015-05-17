#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/fuse.h>
#include "boot-funcs.h"

extern uint8_t __stack[];
void __attribute__((noreturn)) __optiboot_start(void);
void __attribute__((noreturn)) __bootloader_start(void);
void __attribute__((noreturn)) __application_start(void);

FUSES = {
    .low = (unsigned char)~0,
    .high = FUSE_BOOTSZ0 & FUSE_EESAVE & FUSE_SPIEN,
    .extended = FUSE_BODLEVEL0 & FUSE_BODLEVEL1,
};

void __attribute__((used,naked)) __init(uint32_t magic) {
    asm volatile ("eor r1, r1");
    SP = (uint16_t)(&__stack);

    uint8_t mcusr = MCUSR;

    if (__mcusr_mirror == 0 || (mcusr & (_BV(EXTRF) | _BV(PORF) | _BV(BORF)))) {
        __mcusr_mirror = mcusr;
    }

    wdt_reset();
    wdt_enable(WDTO_8S);

    if (!(mcusr & _BV(EXTRF))) {
        MCUSR = 0;

        asm volatile ("lds r2, __mcusr_mirror");

        if ((mcusr & (_BV(PORF) | _BV(BORF))) || magic == 0xc0decafe) {
            __bootloader_start();
            //asm volatile ("jmp __bootloader_start");
            __builtin_unreachable();
        } else {
            __mcusr_mirror = 0;
            __application_start();
            //asm volatile ("jmp __application_start");
            __builtin_unreachable();
        }
    } else {
        __optiboot_start();
        //asm volatile ("jmp __optiboot_start");
        __builtin_unreachable();
    }
}

