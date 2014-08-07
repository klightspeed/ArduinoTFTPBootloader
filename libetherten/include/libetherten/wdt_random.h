#ifndef WDT_H
#define WDT_H

extern void (*wdt_int_vector)(void);
volatile extern uint16_t wdt_tick;
volatile extern uint8_t wdt_triggered;
void wdt_get_random_bytes(void *buf, int len);
void wdt_reset_mcu(void) __attribute__((noreturn));

#endif
