#ifndef BOOTLOADER_H
#define BOOTLOADER_H

extern const uint8_t bootloader_data[] PROGMEM;
extern const uint8_t bootloader_data_end[] PROGMEM;
#define bootloader_data_len (bootloader_data_end - bootloader_data)

#endif
