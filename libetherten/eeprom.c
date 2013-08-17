#include "eeprom.h"
#include "random.h"

struct eeprom_boot_data eeprom_boot_data;

void load_eeprom_data(void) {
    eeprom_read_block(&eeprom_boot_data, EEPROM_BOOT_DATA_START, sizeof(eeprom_boot_data));

    if (eeprom_boot_data.sig[0] != 0x55 || eeprom_boot_data.sig[1] != 0xAA || eeprom_boot_data.structlen != sizeof(eeprom_boot_data)) {
        memset (&eeprom_boot_data, 0, sizeof(eeprom_boot_data));
	get_random_bytes(&eeprom_boot_data.ifconfig.ethconfig.hwaddr, 6);
	eeprom_boot_data.ifconfig.ethconfig.hwaddr.octet[0] = 0xFE;
	eeprom_boot_data.sig[0] = 0x55;
	eeprom_boot_data.sig[1] = 0xAA;
	eeprom_boot_data.structlen = sizeof(eeprom_boot_data);
	eeprom_boot_data.usedhcp = 1;
	strncpy(eeprom_boot_data.firmware_filename, DEFAULT_FW_FILENAME, sizeof(eeprom_boot_data.firmware_filename));
	save_eeprom_data();
    }
}

void save_eeprom_data(void) {
    eeprom_update_block(&eeprom_boot_data, EEPROM_BOOT_DATA_START, sizeof(eeprom_boot_data));
}
