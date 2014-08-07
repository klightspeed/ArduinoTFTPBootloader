#include "libetherten/config.h"
#include "libetherten/w5100.h"
#include <avr/io.h>
#include "libetherten/util.h"

void __attribute__((__naked__)) __w5100_sock_rw_mem(uint8_t socknum, void *buf, int len, uint16_t optype, int offset) {
    asm volatile (
        "andi r21, hi8(%[mask])\n"
        "push r20\n"
        "push r21\n"
        "movw r30, r22\n"
        "movw r26, r20\n"
        "mov r21, r24\n"
        "mov r25, r24\n"
        "subi r25, hi8(-(%[sockreg]))\n"
        "ldi r24, %[txwrptr]\n"
        "sbrc r19, %[opreadram]\n"
        "subi r24, %[txwrptr] - %[rxrdptr]\n"
        "push r24\n"
        "push r25\n"
        "ldi r22, 0xFF\n"
        "ldi r23, 0xFF\n"
        "ldi r20, %[opread]\n"
        "adiw r24, 1\n"
        "call ___w5100_write_byte\n"
        "sbiw r24, 1\n"
        "mov r22, r23\n"
        "in r23, %[datareg]\n"
        "call ___w5100_write_byte\n"
        "mov r24, r23\n"
        "in r25, %[datareg]\n"
        "add r24, r16\n"
        "adc r25, r17\n"
        "push r24\n"
        "push r25\n"
        "andi r25, hi8(%[mask])\n"
        "ldi r22, hi8((%[mask])+1)\n"
        "mul r22, r21\n"
        "mov r21, r0\n"
        "eor r1, r1\n"
        "subi r21, hi8(-(%[txbuf]))\n"
        "sbrc r19, %[opreadram]\n"
        "subi r21, hi8((%[txbuf])-(%[rxbuf]))\n"
        "or r25, r21\n"
        "mov r20, r18\n"
        "adiw r26, 1\n"
        "rjmp 2f\n"
        "1:\n"
        "sbrc r19, %[opwriteram]\n"
        "ld r22, Z+\n"
        "sbrc r19, %[opwriteflash]\n"
        "lpm r22, Z+\n"
        "call ___w5100_write_byte\n"
        "in r22, %[datareg]\n"
        "sbrc r19, %[opreadram]\n"
        "st Z+, r22\n"
        "adiw r24, 1\n"
        "ldi r22, hi8(%[mask])\n"
        "and r22, r25\n"
        "brne 2f\n"
        "or r24, r24\n"
        "brne 2f\n"
        "subi r25, hi8((%[mask])+1)\n"
        "2:\n"
        "sbiw r26, 1\n"
        "brne 1b\n"
        "pop r23\n"
        "pop r22\n"
        "pop r25\n"
        "pop r24\n"
        "pop r21\n"
        "pop r20\n"
        "add r22, r20\n"
        "adc r23, r21\n"
        "ldi r20, %[opwrite]\n"
        "sbrs r19, %[opcmd]\n"
        "rjmp 4f\n"
        "adiw r24, 1\n"
        "call ___w5100_write_byte\n"
        "sbiw r24, 1\n"
        "mov r22, r23\n"
        "call ___w5100_write_byte\n"
        "ldi r24, %[cmdreg]\n"
        "ldi r22, %[cmdrecv]\n"
        "sbrs r19, %[opreadram]\n"
        "subi r22, %[cmdrecv] - %[cmdsend]\n"
        "ldi r20, %[opwrite]\n"
        "call ___w5100_write_byte\n"
        "ldi r20, %[opread]\n"
        "ldi r22, 0xFF\n"
        "3:\n"
        "call ___w5100_write_byte\n"
        "in r0, %[datareg]\n"
        "or r0, r0\n"
        "brne 3b\n"
        "4:\n"
        "ret"
        :
        : [txwrptr] "i" (offsetof(struct w5100_regs_socket, txwrptr)),
          [rxrdptr] "i" (offsetof(struct w5100_regs_socket, rxrdptr)),
          [cmdreg] "i" (offsetof(struct w5100_regs_socket, command)),
          [sockreg] "i" (offsetof(struct w5100_regs, socket)),
          [txbuf] "i" (offsetof(struct w5100_regs, txbuffer)),
          [rxbuf] "i" (offsetof(struct w5100_regs, rxbuffer)),
          [datareg] "I" (_SFR_IO_ADDR(w5100_spi_datareg)),
          [opread] "i" (W5100_OP_READ),
          [opwrite] "i" (W5100_OP_WRITE),
          [opreadram] "I" (W5100_OP_READ_RAM_BIT),
          [opwriteram] "I" (W5100_OP_WRITE_RAM_BIT),
          [opwriteflash] "I" (W5100_OP_WRITE_FLASH_BIT),
          [opcmd] "I" (W5100_OP_CMD_BIT),
          [cmdrecv] "i" (W5100_CR_RECV),
          [cmdsend] "i" (W5100_CR_SEND),
          [mask] "i" (0x7FF)
    );
}

