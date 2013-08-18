#ifndef DHCP_H
#define DHCP_H

#include "config.h"
#include <stdint.h>
#include <string.h>
#include "w5100.h"
#include "random.h"

struct dhcp_packet {
    uint8_t msgtype;
    uint8_t hwtype;
    uint8_t hwaddrlen;
    uint8_t hops;
    uint8_t txid[4];
    uint8_t elapsed[2];
    uint8_t flags[2];
    struct ipaddr clientip;
    struct ipaddr yourip;
    struct ipaddr nextip;
    struct ipaddr relayip;
    struct hwaddr hwaddr;
    uint8_t hwaddr_pad[10];
    char servername[64];
    char bootfilename[128];
#ifdef USE_DHCP
    uint8_t dhcpcookie[4];
    uint8_t options[336];
#else
    uint8_t bootpopts[64];
#endif
};

struct dhcp_ifconfig {
    struct w5100_ifconfig ethconfig;
    struct ipaddr dhcpserver;
#ifdef USE_DHCP
    char hostname[16];
    struct ipaddr dnsserver;
    struct ipaddr bcastaddr;
#endif
};

struct dhcp_state {
    union {
        struct w5100_udp_packet udppacket;
        struct {
	    struct ipaddr serverip;
	    uint16_t serverport;
	    uint16_t packetlen;
	    struct dhcp_packet packet;
	};
    };
};

#define dhcp_init_request(state,ifconfig,reqtype) \
    do { \
	register struct dhcp_state *__state asm("r26") = state; \
	register struct dhcp_ifconfig *__ifconfig asm("r30") = ifconfig; \
	register uint8_t __reqtype asm("r20") = reqtype; \
	asm volatile ( \
	    "call __dhcp_init_request" \
	    : "+z" (__ifconfig) \
	    : "x" (__state), \
              "r" (__reqtype) \
	    : "r23", "memory" \
	); \
    } while (0)

static inline void dhcp_init(struct dhcp_ifconfig *ifconfig, uint8_t socknum) {
    w5100_init(&ifconfig->ethconfig);
    w5100_udp_bind(socknum, 68);
    w5100_set_destipaddr(socknum, &gbcastipaddr);
    w5100_set_destport(socknum, 67);
    w5100_set_desthwaddr(socknum, &bcasthwaddr);
}

static inline void dhcp_send_request(struct dhcp_state *state, struct dhcp_ifconfig *ifconfig, uint8_t reqtype, uint8_t socknum) {
    dhcp_init_request(state, ifconfig, reqtype);
    w5100_send(socknum, &state->packet, state->packetlen);
}

static inline uint8_t dhcp_try_get_response(struct dhcp_state *state, struct dhcp_ifconfig *ifconfig, uint8_t socknum) {
    if (w5100_udp_try_recv(socknum, &state->udppacket)) {

	if (state->packet.msgtype == 2) {
	    if (!compare_zx(&state->packet.hwaddr, &ifconfig->ethconfig.hwaddr, 6)) {
		uint8_t *optptr = state->packet.options;
		uint8_t optcode;
		uint8_t resptype = 0;

		#define ldx() read_rx(optptr,r21)
		#define ldx_str(v,len) copy_xz_constz(optptr,v,len)

		while ((optcode = ldx()) != 255) {
		    if (optcode != 0) {
			uint8_t optlen = ldx();
			uint8_t readlen = 0;
			
			if (optcode == 53) {
			    resptype = ldx();
			    readlen = 1;
			} else if (optcode == 54) {
			    ldx_str(&ifconfig->dhcpserver, 4);
			    readlen = 4;
			} else if (optcode == 1) {
			    ldx_str(&ifconfig->ethconfig.subnet, 4);
			    readlen = 4;
			} else if (optcode == 3) {
			    ldx_str(&ifconfig->ethconfig.gateway, 4);
			    readlen = 4;
			}

			optptr += optlen - readlen;
		    }
		}

		if (resptype == 2 || resptype == 5) {
		    if (compare_const_zx(&state->packet.yourip, "\0\0\0\0", 4)) {
			optptr = (uint8_t *)&state->packet.yourip;
			ldx_str(&ifconfig->ethconfig.ipaddr, 4);
			for (int i = 0; i < 4; i++) {
			    ifconfig->bcastaddr.octet[i] = ifconfig->ethconfig.ipaddr.octet[i] | ((uint8_t)(~ifconfig->ethconfig.subnet.octet[i]));
			}

			return resptype;
		    }
		} else if (resptype == 6) {
		    return resptype;
		}
	    }
	}
    }

    return 0;
}
   

static inline void dhcp_get_address(struct dhcp_state *state, struct dhcp_ifconfig *ifconfig, uint8_t socknum) {
    uint8_t reqtype = 1;
    uint8_t retries = 10;

    dhcp_init(ifconfig, socknum);

    do {
        uint8_t resptype;
	uint8_t recvtries = 100;
	dhcp_send_request(state, ifconfig, socknum, reqtype);
	do {
	    resptype = dhcp_try_get_response(state, ifconfig, socknum);

	    if (resptype == 2) {
	        reqtype = 3;
		retries = 4;
		break;
	    } else if (resptype == 5) {
	        return;
	    } else if (resptype == 6) {
	        reqtype = 1;
		retries = 4;
		break;
	    }

	    _delay_ms(10);
	} while (--recvtries);
    } while (--retries);
}


#endif
