#include "libetherten/config.h"
#include "libetherten/adc.h"
#include "libetherten/util.h"
#include <avr/io.h>
#include <util/delay.h>

void adc_init() {
    ADCSRA = 0;
    ADMUX = _BV(REFS0);
    ADCSRA = _BV(ADEN) | ADC_PRESCALER;
    ADCSRB = 0;
    ACSR = _BV(ACD);
    DDRC = 0;
    PORTC = 0;
    DIDR0 = 0x3F;
}


