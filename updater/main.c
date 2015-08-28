#include "libetherten/config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <string.h>
#include <util/delay.h>
#include "bootloader.h"
#include "libetherten/flash.h"
#include "libetherten/eeprom.h"
#include "libetherten/random.h"

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
    struct eeprom_boot_data eedata;
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    PORTB = 0xFF;
    PORTC = 0xFF;
    PORTD = 0xFF;
    
    DDRB |= _BV(2) | _BV(4) | _BV(5);

    seed_pseudorandom();
    load_eeprom_data(&eedata);
    init_eeprom_data(&eedata);
    save_eeprom_data(&eedata);

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
