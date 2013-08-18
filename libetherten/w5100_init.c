#include "config.h"
#include "w5100.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "util.h"

struct w5100_regs_base w5100_reginit = {
    .mode = 0x80,
    .ifconfig = {
        .gateway = { { 0, 0, 0, 0 } },
	.subnet = { { 0, 0, 0, 0 } },
	.hwaddr = { { 0xFE, 0xFD, 0, 0, 0, 1 } },
	.ipaddr = { { 0, 0, 0, 0 } }
    },

    .intreg = 0,
    .intmask = 0,
    .retrytime = BIGENDWORDBYTES(2000),
    .retrycount = 128,
    .rxmemsize = W5100_BUFSIZE(2k,2k,2k,2k),
    .txmemsize = W5100_BUFSIZE(2k,2k,2k,2k)
};

void w5100_init(struct w5100_ifconfig *ifconfig) {
    static struct w5100_ifconfig _ifconfig;

    W5100_PORT |= _BV(W5100_SS_BIT);
    W5100_DDR |= _BV(W5100_SS_BIT);

    w5100_spi_setup(W5100_SPI_CLK_DIV);

    if (ifconfig != NULL) {
        copy_zx_const (&w5100_reginit.ifconfig, ifconfig, sizeof w5100_reginit.ifconfig);
    }

    do {
	_delay_ms(200);
       
	w5100_write_mem(0, &w5100_reginit, sizeof w5100_reginit);
	w5100_read_mem(1, &_ifconfig, sizeof _ifconfig);
    } while (compare_zx(&_ifconfig, ifconfig, sizeof _ifconfig));
}

