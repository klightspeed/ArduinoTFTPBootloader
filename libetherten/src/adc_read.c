#include "libetherten/config.h"
#include "libetherten/adc.h"
#include "libetherten/util.h"
#include <avr/io.h>
#include <util/delay.h>

uint16_t adc_read(uint8_t channel, uint8_t ref) {
    uint8_t lo, hi;
    ADMUX = _BV(REFS0) | (ref ? _BV(REFS1) : 0) | channel;
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & _BV(ADSC));
    lo = ADCL;
    hi = ADCH;
    return MAKEWORD(lo, hi);
}

