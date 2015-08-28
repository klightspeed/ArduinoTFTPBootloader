#include "config.h"
#include "libetherten/config.h"
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
#include "libetherten/spi.h"
#include "libetherten/w5100.h"
#include "libetherten/util.h"
#include "libetherten/random.h"
#include "libetherten/eeprom.h"
#include "libetherten/dhcp.h"
#include "libetherten/tftp.h"
#include "libetherten/sdcard.h"
#include "libetherten/flash.h"
#include "multiboot.h"
#include "boot-funcs.h"
#include "writeflash.h"

static union {
    struct dhcp_state dhcp;
    struct tftp_state tftp;
} state;

int main (void) {
    struct eeprom_boot_data eeprom_boot_data;
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
#if defined(CONFIG_DHCP_RANDOM_XID) || (defined(CONFIG_INIT_EEPROM_BOOTDATA) && defined(CONFIG_RANDOM_HWADDR))
    seed_pseudorandom();
#endif

    MCUCR = (1<<IVCE);
    MCUCR = (1<<IVSEL);

    sdcard_init();

    load_eeprom_data(&eeprom_boot_data);

#ifdef CONFIG_INIT_EEPROM_BOOTDATA
    init_eeprom_data(&eeprom_boot_data);
#endif

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
            copy_tftp_flash(&state.tftp, 2);
        }
    }

    __reboot_application();
    __builtin_unreachable();
}

