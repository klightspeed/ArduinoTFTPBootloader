#include <avr/boot.h>
#include <util/atomic.h>
#include <avr/wdt.h>
#include "util.h"

extern void *__vectors;

void ___flash_write_page(void *pageaddr, void *src, uint16_t valid1, uint16_t valid2) {
    if ((valid1 ^ (uint16_t)pageaddr) == 0xc0de && (valid2 ^ (uint16_t)src) == 0xcafe && (uint16_t)pageaddr < (uint16_t)(&__vectors)) {
        pageaddr = (void *)((uint16_t)pageaddr & -SPM_PAGESIZE);

        if (compare_const_zx(src, pageaddr, SPM_PAGESIZE)) {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                register uint16_t __pageaddr asm("r30") = (uint16_t)pageaddr;
                register void *__src asm("r28") = src;

                wdt_reset();
                boot_spm_busy_wait();
                eeprom_busy_wait();
                boot_page_erase(__pageaddr);
                boot_spm_busy_wait();
                wdt_reset();

                do {
                    asm volatile (
                        "ld r0, Y+\n\t"
                        "ld r1, Y+\n\t"
                        "sts %[spmreg], %[pagefill]\n\t"
                        "spm\n\t"
                        "clr r1\n\t"
                        "adiw r30, 2\n\t"
                        : "+y" (__src), "+z" (__pageaddr)
                        : [spmreg] "i" (_SFR_MEM_ADDR(__SPM_REG)),
                          [pagefill] "r" ((uint8_t)__BOOT_PAGE_FILL)
                        : "r0"
                    );
                } while (LO8(__pageaddr) & (uint8_t)(~(-(SPM_PAGESIZE))));

                __pageaddr = (uint16_t)pageaddr;

                boot_page_write(__pageaddr);
                boot_spm_busy_wait();
                boot_rww_enable();
            }
        }
    }
}

