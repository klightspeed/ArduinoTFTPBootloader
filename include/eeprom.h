#ifndef EEPROM_H
#define EEPROM_H

#include "config.h"
#include <stdint.h>
#include <avr/eeprom.h>
#include "w5100.h"
#include "dhcp.h"

struct eeprom_boot_data {
    uint8_t sig[2];
    uint8_t structlen;
    struct dhcp_ifconfig ifconfig;
    uint8_t usedhcp;
    struct {
        uint32_t epoch;
        uint32_t sec;
    } startup_time;
    char firmware_filename[64];
};

extern struct eeprom_boot_data __eeprom_boot_data EEMEM;
extern struct eeprom_boot_data eeprom_boot_data;

void load_eeprom_data(void);
void save_eeprom_data(void);

#endif
