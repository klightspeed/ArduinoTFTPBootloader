#include "spi_spi0.h"

void __spi_spi0_wait() {
    while(!(SPSR & _BV(SPIF)));
}


