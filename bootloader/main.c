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

int main (void) {
    char *firmware_filename = NULL;

    MCUCR = (1<<IVCE);
    MCUCR = (1<<IVSEL);

    sdcard_init();

    load_eeprom_data();

    if (eeprom_boot_data.usedhcp) {
        dhcp_get_address(&state.dhcp, &eeprom_boot_data.ifconfig);
    }

    if (compare_const_zx(&eeprom_boot_data.ifconfig.ethconfig.ipaddr, PSTR("\0\0\0\0"), 4)) {
	w5100_init(&eeprom_boot_data.ifconfig.ethconfig);
	/*
	w5100_udp_bind(3, 9);
	w5100_set_destipaddr(3, &gbcastipaddr);
	w5100_set_desthwaddr(3, &bcasthwaddr);
	w5100_set_destport(3, 9);
	 */

	if (eeprom_boot_data.firmware_filename[0] != 0 && 
	    tftp_open(&state.tftp, &eeprom_boot_data.ifconfig.bcastaddr, eeprom_boot_data.firmware_filename, 2)) {
	    firmware_filename = eeprom_boot_data.firmware_filename;
	} else if (tftp_open(&state.tftp, &eeprom_boot_data.ifconfig.bcastaddr, DEFAULT_FW_FILENAME, 2)) {
	    firmware_filename = DEFAULT_FW_FILENAME;
	} else if (tftp_open(&state.tftp, &eeprom_boot_data.ifconfig.bcastaddr, DEFAULT_FW_FILENAME2, 2)) {
	    firmware_filename = DEFAULT_FW_FILENAME2;
	}

	if (firmware_filename) {
	    uint16_t blknum = 0;
	    uint8_t flashchanged = 0;
	    uint8_t filevalid = 1;

	    do {
		uint16_t datalen = state.tftp.packetlen - 4;
		uint8_t *data = state.tftp.packet.data;


		if (blknum < ((32768 - 4096) / 512)) {
		    if (compare_const_zx(data, (void *)(blknum * 512), datalen)) {
			flashchanged = 1;
		    }
		} else {
		    filevalid = 0;
		    break;
		}
	    } while (tftp_read_block(&state.tftp, ++blknum, 2));

	    if (flashchanged && filevalid) {
		blknum = 0;
		tftp_open(&state.tftp, &eeprom_boot_data.ifconfig.bcastaddr, firmware_filename, 2);

		do {
		    uint16_t datalen = state.tftp.packetlen - 4;
		    void *data = state.tftp.packet.data;
		    uint16_t sectpage = blknum * 512;

		    for (int pageaddr = 0; pageaddr < datalen; pageaddr += SPM_PAGESIZE) {
			flash_write_page((void *)(sectpage + pageaddr), data + pageaddr);
		    }
		} while (tftp_read_block(&state.tftp, ++blknum, 2));
	    }
	}
    }

    __reboot_application();

    return 0;
}

