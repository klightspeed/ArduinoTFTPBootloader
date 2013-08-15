#ifndef SDCARD_H
#define SDCARD_H

#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "spi.h"
#include "util.h"


#define sdcard_spi_datareg spi_spi0_datareg
#define sdcard_spi_write(v) spi_spi0_write(v)
#define sdcard_spi_wait() spi_spi0_wait()
#define sdcard_spi_read() spi_spi0_read()
#define sdcard_spi_setup(div) spi_spi0_setup(div)

#define sdcard_init() \
    do { \
        const char *cmd = PSTR("\x40\x00\x00\x00\x00\x95\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"); \
	sdcard_spi_setup(128); \
	PORTB |= SDCARDS_PORTB; \
	PORTD |= SDCARDS_PORTD; \
	DDRB |= SDCARDS_PORTB; \
	DDRD |= SDCARDS_PORTD; \
	for (int __i = 0; __i < 16; __i++) { \
	    sdcard_spi_write(0xFF); \
	    sdcard_spi_wait(); \
	} \
	PORTB &= ~SDCARDS_PORTB; \
	PORTD &= ~SDCARDS_PORTD; \
	for (int __i = 0; __i < 16; __i++) { \
	    uint8_t b; \
	    asm volatile ("lpm %0, Z+" : "=r" (b), "+z" (cmd)); \
	    sdcard_spi_write(b); \
	    sdcard_spi_wait(); \
	} \
	PORTB |= SDCARDS_PORTB; \
	PORTD |= SDCARDS_PORTD; \
	sdcard_spi_write(0xFF); \
	sdcard_spi_wait(); \
    } while (0)

#endif
