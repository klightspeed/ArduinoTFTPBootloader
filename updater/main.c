#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <string.h>
#include <util/delay.h>
#include "bootloader.h"
#include "flash.h"

void __bootloader_start(void);

static int memcmp_PP(const void *src, const void *dst, int len) {
    while (len--) {
        if (pgm_read_byte_near(src++) != pgm_read_byte_near(dst++)) {
	    return 1;
	}
    }

    return 0;
}

static uint8_t flashdata[SPM_PAGESIZE];

int main (void) {
    if (memcmp_PP(bootloader_data, (void *)__bootloader_start, bootloader_data_len)) {
        DDRB |= _BV(PB5);
	
	for (int pageaddr = 0; pageaddr < bootloader_data_len; pageaddr += SPM_PAGESIZE) {
	    PORTB ^= _BV(PB5);

	    memcpy_P(flashdata, bootloader_data + pageaddr, SPM_PAGESIZE);

	    flash_write_page((void *)__bootloader_start + pageaddr, flashdata);
	}
    }

    while (1) {
	PORTB ^= _BV(PB5);
	_delay_ms(500);
    }
}
