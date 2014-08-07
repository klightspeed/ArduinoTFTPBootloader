#ifndef SPI_SPI0_H
#define SPI_SPI0_H

#include "config.h"
#include <avr/io.h>
#include <avr/cpufunc.h>

#define SPI_SPI0_PORT     PORTB
#define SPI_SPI0_DDR      DDRB
#define SPI_SPI0_PIN      PINB
#define SPI_SPI0_SCK_BIT  PORTB5
#define SPI_SPI0_MISO_BIT PORTB4
#define SPI_SPI0_MOSI_BIT PORTB3

#ifdef __ASSEMBLER__

.macro spi_spi0_write r
    out _SFR_IO_ADDR(SPDR), \r
  1:
    in r0, _SFR_IO_ADDR(SPSR)
    sbrs r0, SPIF
    rjmp 1b
.endm

.macro spi_spi0_read r
    in \r, _SFR_IO_ADDR(SPDR)
.endm

#else /* __ASSEMBLER__ */

static inline void spi_spi0_setup(int div) {
    uint8_t clkdiv2;
    uint8_t clkdiv;

    if (div == 2 || div == 8 || div == 32) {
        clkdiv2 = _BV(SPI2X);
        div = div << 1;
    }

    if (div == 4) {
        clkdiv = 0;
    } else if (div == 16) {
        clkdiv = _BV(SPR0);
    } else if (div == 64) {
        clkdiv = _BV(SPR1);
    } else if (div == 128) {
        clkdiv = _BV(SPR0) | _BV(SPR1);
    } else {
        spi_spi0_setup(DEFAULT_SPI_CLK_DIV);
        return;
    }

    SPCR = 0;
    SPSR = clkdiv2;
    SPCR = _BV(SPE) | _BV(MSTR) | clkdiv;
    SPI_SPI0_DDR = (SPI_SPI0_DDR & ~_BV(SPI_SPI0_MISO_BIT)) | _BV(SPI_SPI0_MOSI_BIT) | _BV(SPI_SPI0_SCK_BIT);
    SPI_SPI0_PORT |= SPI_SPI0_MISO_BIT;
}

#define SPI_SPI0_PORT_SET(b) SPI_SPI0_PORT |= _BV(SPI_SPI0_ ## b ## _BIT)
#define SPI_SPI0_PORT_CLR(b) SPI_SPI0_PORT &= ~_BV(SPI_SPI0_ ## b ## _BIT)
#define SPI_SPI0_DDR_OUT(b)  SPI_SPI0_DDR  |= _BV(SPI_SPI0_ ## b ## _BIT)
#define SPI_SPI0_DDR_IN(b)   SPI_SPI0_DDR  &= ~_BV(SPI_SPI0_ ## b ## _BIT)
#define SPI_SPI0_PIN_SET(b)  (SPI_SPI0_PIN & _BV(SPI_SPI0_ ## b ## _BIT)

#define spi_spi0_datareg     SPDR
#define spi_spi0_write(b)    spi_spi0_datareg = (b)
#define spi_spi0_wait()      asm volatile ("call __spi_spi0_wait")
#define spi_spi0_read()      spi_spi0_datareg

#endif /* __ASSEMBLER__ */

#endif /* SPI_SPI0_H */
