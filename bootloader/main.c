#include "config.h"
#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include "spi.h"
#include "w5100.h"
#include "util.h"
#include "random.h"
#include "eeprom.h"
#include "dhcp.h"
#include "tftp.h"
#include "sdcard.h"
#include "multiboot.h"
#include "flash.h"
#include "boot-funcs.h"

static union {
    struct dhcp_state dhcp;
    struct tftp_state tftp;
} state;

void write_flash_sector(void *data, uint16_t sectpage, int datalen) {
    if (sectpage < 28672) {
        for (int pageaddr = 0; pageaddr < datalen; pageaddr += SPM_PAGESIZE) {
            if (compare_const_zx(data, (void *)(sectpage + pageaddr), SPM_PAGESIZE)) {
                wdt_reset();
                flash_write_page((void *)(sectpage + pageaddr), data + pageaddr);
            }
        }
    }
}

void copy_tftp_flash(uint8_t socknum) {
    uint16_t blknum = 0;

    memset (&state, 0, sizeof(state));
    while (tftp_read_block(&state.tftp, blknum, 2)) {
        uint16_t datalen = state.tftp.packetlen - 4;
        uint8_t *data = state.tftp.packet.data;
        uint16_t sectpage = blknum * 512;

        if (sectpage < 28672) {
            write_flash_sector(data, sectpage, datalen);
        } else {
            break;
        }

        blknum++;
    } 

    __reboot_application();
}

void copy_tcp_flash(uint8_t socknum) {
    uint16_t pos = 0;
    uint8_t data[SPM_PAGESIZE];

    while (pos < 28672) {
        memset (data, 0, sizeof(data));
        int len = w5100_tcp_recv(socknum, data, SPM_PAGESIZE, SPM_PAGESIZE, 1000);
        
        if (len) {
            write_flash_sector(data, pos, len);
            pos += len;
        }

        if (len != SPM_PAGESIZE) {
            w5100_sock_close(socknum);
            break;
        }

        w5100_send_p(socknum, (void *)PSTR(""), 1);
    }

    __reboot_application();
}

int main (void) {
    char *try_filenames[] = {
        eeprom_boot_data.firmware_filename,
        DEFAULT_FW_FILENAME,
        DEFAULT_FW_FILENAME2,
        NULL
    };
    char **try_filename = try_filenames;
    char *firmware_filename = NULL;

    wdt_reset();
    wdt_enable(WDTO_1S);
    seed_pseudorandom();

    MCUCR = (1<<IVCE);
    MCUCR = (1<<IVSEL);

    sdcard_init();

    load_eeprom_data();

    if (eeprom_boot_data.usedhcp) {
        dhcp_get_address(&state.dhcp, &eeprom_boot_data.ifconfig, 2);
    }

    wdt_reset();

    if (compare_const_zx(&eeprom_boot_data.ifconfig.ethconfig.ipaddr, PSTR("\0\0\0\0"), 4)) {
        w5100_init(&eeprom_boot_data.ifconfig.ethconfig);
        /*
        w5100_udp_bind(3, 9);
        w5100_set_destipaddr(3, &gbcastipaddr);
        w5100_set_desthwaddr(3, &bcasthwaddr);
        w5100_set_destport(3, 9);
         */

        while (*try_filename != NULL) {
            if (*try_filename[0] != 0 && tftp_open(&state.tftp, &eeprom_boot_data.ifconfig.bcastaddr, *try_filename, 2)) {
                firmware_filename = *try_filenames;
                break;
            }

            try_filename++;
        }

        wdt_reset();

        if (firmware_filename) {
            copy_tftp_flash(2);
        }
    }

    __reboot_application();

    return 0;
}

