#include "config.h"
#include "w5100.h"
#include "util.h"

void ___w5100_write_byte(uint16_t address, uint8_t value, uint8_t op) {
    union twobyte addr;

    addr.word = address;
    w5100_select();
    w5100_spi_write(op);
    w5100_spi_wait();
    w5100_spi_write(addr.byte[1]);
    w5100_spi_wait();
    w5100_spi_write(addr.byte[0]);
    w5100_spi_wait();
    w5100_spi_write(value);
    w5100_spi_wait();
    w5100_deselect();
}


