#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include "boot-funcs.h"

void __attribute__((naked,noreturn)) ___wdt_reset_mcu(uint32_t magic) {
    asm volatile (
        "cli\n\t"
        "wdr\n\t"
        "ldi r18, %1\n\t"
        "ldi r19, %2\n\t"
        "sts %0, r18\n\t"
        "sts %0, r19\n\t"
        :
        : "i" (_SFR_MEM_ADDR(_WD_CONTROL_REG)),
          "M" (_BV(_WD_CHANGE_BIT) | _BV(WDE)),
          "M" (_BV(WDE))
        : "r18", "r19"
    );

    while (1);
}

void ___reboot_application(void) {
    ___wdt_reset_mcu(0xfeedf00d);
}

void ___reboot_bootloader(void) {
    ___wdt_reset_mcu(0xc0decafe);
}
