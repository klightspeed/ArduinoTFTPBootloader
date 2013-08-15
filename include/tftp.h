#ifndef TFTP_H
#define TFTP_H

#include "config.h"
#include <stdint.h>
#include "w5100.h"

struct tftp_packet {
    uint8_t opcode[2];
    union {
        char filename_options[514];
	struct {
	    uint8_t blknum[2];
	    uint8_t data[512];
	};
    };
};

struct tftp_state {
    union {
        struct w5100_udp_packet udppacket;
	struct {
	    struct ipaddr serverip;
	    uint16_t serverport;
	    uint16_t packetlen;
	    struct tftp_packet packet;
	};
    };
};

extern struct tftp_state tftp_state;

uint8_t tftp_open(struct ipaddr *ipaddr, char *filename);
uint8_t tftp_read_block(uint16_t blknum);

#define tftp_close() w5100_sock_close(2)

#endif
