#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <avr/sleep.h>
#include <util/atomic.h>

void wdt_get_random_bytes(void *buf, int len) {
    uint8_t oldwdtcr;
    uint8_t i, j;
    uint8_t entropy = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
	TCCR2A = 0;
	TCCR2B = _BV(CS20);

	oldwdtcr = _WD_CONTROL_REG;
	MCUSR = 0;
	wdt_reset();
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = _BV(WDIE);

	for (i = 0; i < len; i++) {
	    for (j = 0; j < 8; j++) {
	        while (!(_WD_CONTROL_REG & _BV(WDIF)));

		_WD_CONTROL_REG &= ~_BV(WDIF);

		asm volatile (
		    "lsl %0\n\t"
		    "adc %0, __zero_reg__\n\t"
		    "eor %0, %1\n\t"
		    : "+r" (entropy)
		    : "r" (TCNT2)
		);
	    }

	    *((uint8_t *)(buf + i)) = entropy;
	}

	wdt_reset();
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = oldwdtcr;
    }
}


