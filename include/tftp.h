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

static inline void tftp_init(struct ipaddr *ipaddr, uint8_t socknum) {
    w5100_udp_bind(socknum, 49152);
    w5100_set_destipaddr(socknum, ipaddr);
    w5100_set_destport(socknum, 69);
    
    if (ipaddr->octet[3] == 255) {
        w5100_set_desthwaddr(socknum, &bcasthwaddr);
    }
}

static inline void tftp_send_rrq(struct tftp_state *state, char *filename, uint8_t socknum) {
    uint8_t *buf = (uint8_t *)&state->packet;
    write_x(buf, 0);
    write_x(buf, 1);
    copy_zx_str(buf, filename, 64);
    copy_const_zx_constz(buf, PSTR("octet\000blksize\000512\000"), 18);

    w5100_send(socknum, &state->packet, buf - (uint8_t *)&state->packet);
}

static inline void tftp_send_ack(struct tftp_state *state, uint16_t blknum, uint8_t socknum) {
    uint8_t *buf = (uint8_t *)&state->packet;
    union twobyte __blknum;
    __blknum.word = blknum;
    write_x(buf, 0);
    write_x(buf, 4);
    write_x(buf, __blknum.byte[1]);
    write_x(buf, __blknum.byte[0]);

    w5100_send(socknum, &state->packet, 4);
}

static inline uint8_t tftp_try_get_response(struct tftp_state *state, uint16_t blknum, uint8_t socknum) {
    if (w5100_udp_try_recv(socknum, &state->udppacket)) {
	if (state->packet.opcode[1] == 6) {
	    w5100_set_destipaddr(socknum, &state->serverip);
	    w5100_set_destport(socknum, state->serverport);
	    tftp_send_ack(state, 0, socknum);
	} else if (state->packet.opcode[1] == 3) {
            union twobyte __blknum;
	    __blknum.byte[1] = state->packet.blknum[0];
	    __blknum.byte[0] = state->packet.blknum[1];
	    
	    if (__blknum.word == blknum) {
	        return 1;
	    } else if (__blknum.word < blknum) {
		state->packet.opcode[1] = 4;
		w5100_send(socknum, &state->packet, 4);
	    }
	}
    }

    return 0;
}

static inline uint8_t tftp_open(struct tftp_state *state, struct ipaddr *ipaddr, char *filename, uint8_t socknum) {
    uint8_t retries = 2;
    tftp_init(ipaddr, socknum);

    do {
        uint8_t recvtries = 50;

	wdt_reset();
        tftp_send_rrq(state, filename, socknum);
	do {
	    wdt_reset();
	    if (tftp_try_get_response(state, 1, socknum)) {
	        return 1;
	    }

	    _delay_ms(10);
	} while (--recvtries);
    } while (--retries);

    return 0;
}

static inline uint8_t tftp_read_block(struct tftp_state *state, uint16_t blknum, uint8_t socknum) {
    uint8_t retries = 5;

    do {
        uint8_t recvtries = 50;
	wdt_reset();
	tftp_send_ack(state, blknum, socknum);

	do {
	    wdt_reset();
	    if (tftp_try_get_response(state, blknum + 1, socknum)) {
		if (state->packetlen == 4) {
		    state->packet.opcode[1] = 4;
		    w5100_send(socknum, &state->packet, 4);
		    w5100_sock_close(socknum);
		    return 0;
		}

	        return 1;
	    }

	    _delay_ms(10);
	} while (--recvtries);
    } while (--retries);

    return 0;
}

#define tftp_close() w5100_sock_close(2)

#endif
