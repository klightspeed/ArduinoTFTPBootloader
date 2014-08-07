#ifndef WRITEFLASH_H
#define WRITEFLASH_H

#include "config.h"
#include "libetherten/config.h"
#include <stdint.h>
#include "libetherten/tftp.h"

void copy_tcp_flash(uint8_t socknum);
void copy_tftp_flash(struct tftp_state *state, uint8_t socknum);

#endif
