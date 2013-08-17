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

#ifdef USE_DHCP
#define BCASTIPADDR &eeprom_boot_data.ifconfig.bcastaddr
#else
#define BCASTIPADDR &gbcastipaddr
#endif

static struct dhcp_state dhcp_state;

int main (void) {
    char *firmware_filename = NULL;

    MCUCR = (1<<IVCE);
    MCUCR = (1<<IVSEL);

#ifdef SEND_SD_IDLE
    sdcard_init();
#endif

    sei();

    load_eeprom_data();

    if (eeprom_boot_data.usedhcp) {
        dhcp_get_address(&dhcp_state, &eeprom_boot_data.ifconfig);
    }

    if (memcmp(&eeprom_boot_data.ifconfig.ethconfig.ipaddr, "\0\0\0\0", 4)) {
	w5100_init(&eeprom_boot_data.ifconfig.ethconfig);
	/*
	w5100_udp_bind(3, 9);
	w5100_set_destipaddr(3, &gbcastipaddr);
	w5100_set_desthwaddr(3, &bcasthwaddr);
	w5100_set_destport(3, 9);
	 */

	if (eeprom_boot_data.firmware_filename[0] != 0 && 
	    tftp_open(BCASTIPADDR, eeprom_boot_data.firmware_filename)) {
	    firmware_filename = eeprom_boot_data.firmware_filename;
	} else if (tftp_open(BCASTIPADDR, DEFAULT_FW_FILENAME)) {
	    firmware_filename = DEFAULT_FW_FILENAME;
	} else if (tftp_open(BCASTIPADDR, DEFAULT_FW_FILENAME2)) {
	    firmware_filename = DEFAULT_FW_FILENAME2;
	}

	if (firmware_filename) {
	    uint16_t blknum = 0;
	    uint8_t flashchanged = 0;
	    uint8_t filevalid = 1;

	    do {
		uint16_t datalen = tftp_state.packetlen - 4;
		uint8_t *data = tftp_state.packet.data;


		if (blknum < ((32768 - 4096) / 512)) {
		    if (memcmp_P(data, (void *)(blknum * 512), datalen)) {
			flashchanged = 1;
		    }
		} else {
		    filevalid = 0;
		    break;
		}
	    } while (tftp_read_block(++blknum));

	    if (flashchanged && filevalid) {
		blknum = 0;
		tftp_open(BCASTIPADDR, firmware_filename);

		do {
		    uint16_t datalen = tftp_state.packetlen - 4;
		    void *data = tftp_state.packet.data;
		    uint16_t sectpage = blknum * 512;

		    for (int pageaddr = 0; pageaddr < datalen; pageaddr += SPM_PAGESIZE) {
			flash_write_page((void *)(sectpage + pageaddr), data + pageaddr);
		    }
		} while (tftp_read_block(++blknum));
	    }
	}
    }

    cli();

    __reboot_application();

    return 0;
}

