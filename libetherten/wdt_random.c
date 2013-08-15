#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <avr/sleep.h>
#include <util/atomic.h>

volatile uint8_t wdt_tick;
volatile uint8_t wdt_triggered;

ISR(WDT_vect)
{
    wdt_tick = TCNT2;
    wdt_triggered = 1;
}

void wdt_get_random_bytes(void *buf, int len) {
    uint8_t oldwdtcr;
    uint8_t i, j;
    uint8_t entropy = 0;

    TCCR2A = 0;
    TCCR2B = _BV(CS20);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
	oldwdtcr = _WD_CONTROL_REG;
	MCUSR = 0;
	wdt_reset();
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = _BV(WDIE);
    }

    for (i = 0; i < len; i++) {
        
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
	    for (j = 0; j < 8; j++) {
		wdt_triggered = 0;
		
		while (!wdt_triggered) {
		    set_sleep_mode(SLEEP_MODE_IDLE);
		    sleep_enable();
		    sei();
		    sleep_cpu();
		    sleep_disable();
		    cli();
		}

		asm volatile (
		    "lsl %0\n\t"
		    "adc %0, __zero_reg__\n\t"
		    "eor %0, %1\n\t"
		    : "+r" (entropy)
		    : "r" (wdt_tick)
		);
	    }
	}

	*((uint8_t *)(buf + i)) = entropy;
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
	wdt_reset();
	_WD_CONTROL_REG |= _BV(_WD_CHANGE_BIT) | _BV(WDE);
	_WD_CONTROL_REG = oldwdtcr;
    }
}


