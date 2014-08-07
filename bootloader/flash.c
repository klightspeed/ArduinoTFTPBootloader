#include "config.h"
#include <stdint.h>
#include "flash.h"
#include "w5100.h"
#include "tftp.h"
#include "boot-funcs.h"
#include "writeflash.h"

void copy_tftp_flash(struct tftp_state *state, uint8_t socknum) {
    uint16_t blknum = 0;

    memset (state, 0, sizeof(struct tftp_state));
    while (tftp_read_block(state, blknum, 2)) {
        uint16_t datalen = state->packetlen - 4;
        uint8_t *data = state->packet.data;
        uint16_t sectpage = blknum * 512;

        if (sectpage < 28672) {
            for (int pageaddr = 0; pageaddr < datalen; pageaddr += SPM_PAGESIZE) {
                if (compare_const_zx(data + pageaddr, (void *)(sectpage + pageaddr), SPM_PAGESIZE)) {
                    wdt_reset();
                    flash_write_page((void *)(sectpage + pageaddr), data + pageaddr);
                }
            }
        } else {
            break;
        }

        blknum++;
    } 

    __reboot_application();
}

void copy_tcp_flash(uint8_t socknum) {
    uint16_t pos = 0;
    uint8_t data[SPM_PAGESIZE];

    while (pos < 28672) {
        memset (data, 0, sizeof(data));
        int len = w5100_tcp_recv(socknum, data, SPM_PAGESIZE, SPM_PAGESIZE, 1000);
        
        if (len) {
            if (compare_const_zx(data, (void *)pos, len)) {
                wdt_reset();
                flash_write_page((void *)pos, data);
            }

            pos += len;
        }

        if (len != SPM_PAGESIZE) {
            w5100_sock_close(socknum);
            break;
        }

        w5100_send_p(socknum, (void *)PSTR(""), 1, 0);
    }

    __reboot_application();
}

