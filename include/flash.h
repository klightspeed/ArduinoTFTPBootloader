#ifndef FLASH_H
#define FLASH_H

#include "config.h"
#include <avr/boot.h>
#include "flash.h"

void __flash_write_page(void *pageaddr,void *src,uint16_t valid1,uint16_t valid2);

static inline void flash_write_page(void *pageaddr, void *src) {
    __flash_write_page(pageaddr, src, (uint16_t)pageaddr ^ 0xc0de, (uint16_t)src ^ 0xcafe);
}

#endif
