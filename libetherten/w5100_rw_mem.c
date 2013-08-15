#include "config.h"
#include "w5100.h"
#include <avr/io.h>

void __attribute__((__naked__)) __w5100_rw_mem(uint16_t address, void *buf, int len, uint16_t optype) {
    asm volatile (
        "movw r30, r22\n"
	"movw r26, r20\n"
	"movw r20, r18\n"
	"adiw r26, 1\n"
	"rjmp 2f\n"
	"1:\n"
	"sbrc r21, %[opwriteram]\n"
	"ld r22, Z+\n"
	"sbrc r21, %[opwriteflash]\n"
	"lpm r22, Z+\n"
	"call ___w5100_write_byte\n"
	"in r22, %[datareg]\n"
	"sbrc r21, %[opreadram]\n"
	"st Z+, r22\n"
	"adiw r24, 1\n"
	"2:\n"
	"sbiw r26, 1\n"
	"brne 1b\n"
	"ret\n"
	:
	: [opwriteram] "I" (W5100_OP_WRITE_RAM_BIT),
	  [opwriteflash] "I" (W5100_OP_WRITE_FLASH_BIT),
	  [opreadram] "I" (W5100_OP_READ_RAM_BIT),
	  [datareg] "I" (_SFR_IO_ADDR(w5100_spi_datareg))
    );
}


