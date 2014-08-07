#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "util.h"

volatile uint32_t time_epoch;
volatile uint32_t time_sec;
volatile uint16_t time_ms;
//asm (".section .data\n\t.global time_tick\n\ttime_tick = %[tcnt1]" : : [tcnt1] "i" (_SFR_MEM_ADDR(TCNT1)));

void __attribute__((used,naked,section(".init8"))) __init_timer1(void) {
    uint8_t sreg = SREG;
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A = (F_CPU / 1000) - 1;
    TCCR1B |= _BV(WGM12) | _BV(CS10);
    TIMSK1 |= _BV(OCIE1A);
    SREG = sreg;
}

ISR(TIMER1_COMPA_vect)
{
    time_ms++;
    if (unlikely(time_ms >= 1000)) {
        time_ms = 0;
        time_sec++;
        if (unlikely(time_sec == 0)) {
            time_epoch++;
        }
    }
}

