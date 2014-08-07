#include "eeprom.h"
#include "random.h"
#include "util.h"

struct eeprom_boot_data eeprom_boot_data;

void load_eeprom_data(void) {
    eeprom_read_block(&eeprom_boot_data, EEPROM_BOOT_DATA_START, sizeof(eeprom_boot_data));

    if (eeprom_boot_data.sig[0] != 0x55 || eeprom_boot_data.sig[1] != 0xAA || eeprom_boot_data.structlen != sizeof(eeprom_boot_data)) {
        zero_x_const (&eeprom_boot_data, sizeof(eeprom_boot_data));
#ifdef CONFIG_RANDOM_HWADDR
	get_random_bytes(&eeprom_boot_data.ifconfig.ethconfig.hwaddr, 6);
#endif
	eeprom_boot_data.ifconfig.ethconfig.hwaddr.octet[0] = 0xFE;
	eeprom_boot_data.sig[0] = 0x55;
	eeprom_boot_data.sig[1] = 0xAA;
	eeprom_boot_data.structlen = sizeof(eeprom_boot_data);
	eeprom_boot_data.usedhcp = 1;
	copy_const_zx_str_const(eeprom_boot_data.firmware_filename, DEFAULT_FW_FILENAME, sizeof(eeprom_boot_data.firmware_filename));
	save_eeprom_data();
    }
}

void save_eeprom_data(void) {
    eeprom_update_block(&eeprom_boot_data, EEPROM_BOOT_DATA_START, sizeof(eeprom_boot_data));
}
