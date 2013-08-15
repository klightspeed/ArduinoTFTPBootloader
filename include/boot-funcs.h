#ifndef BOOT_FUNCS_H
#define BOOT_FUNCS_H

#include <stdint.h>

extern uint8_t __mcusr_mirror;

void __attribute__((noreturn)) __wdt_reset_mcu(uint32_t magic);
void __attribute__((noreturn)) __reboot_application(void);
void __attribute__((noreturn)) __reboot_bootloader(void);
void __flash_write_page(void *pageaddr, void *src, uint16_t valid1, uint16_t valid2);

#endif
