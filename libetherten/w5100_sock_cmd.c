#include "config.h"
#include "w5100.h"
#include <avr/io.h>

void __attribute__((__naked__)) __w5100_sock_cmd(uint8_t socknum, uint8_t cmd) {
    asm volatile (
        "mov r25, r24\n\t"
        "ori r25, 4\n\t"
        "ldi r24, %[cmdreg]\n\t"
        "ldi r20, %[opwrite]\n\t"
        "call ___w5100_write_byte\n\t"
        "ldi r22, 0xFF\n\t"
        "ldi r20, %[opread]\n\t"
        "1:\n\t"
        "call ___w5100_write_byte\n\t"
        "in r0, %[datareg]\n\t"
        "or r0, r0\n\t"
        "brne 1b\n\t"
        "ret\n\t"
        :
        : [cmdreg] "M" (offsetof(struct w5100_regs_socket, command)),
          [opread] "M" (W5100_OP_READ),
          [opwrite] "M" (W5100_OP_WRITE),
          [datareg] "I" (_SFR_IO_ADDR(w5100_spi_datareg))
    );
}

