#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/atomic.h>

void wdt_reset_mcu(void) {
    cli();
    wdt_enable(WDTO_30MS);
    while (1);
}

