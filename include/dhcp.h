#ifndef DHCP_H
#define DHCP_H

#include "config.h"
#include <stdint.h>
#include <string.h>
#include "w5100.h"
#include "wdt.h"

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

extern struct dhcp_state dhcp_state;

void dhcp_init_request(struct dhcp_ifconfig *ifconfig, uint8_t reqtype, uint8_t elapsed);

static inline void dhcp_get_address(struct dhcp_ifconfig *ifconfig) {

    //static struct ipaddr debugaddr = { { 192, 168, 200, 16 } };
    uint8_t reqtype = 1;
    uint8_t elapsed = 0;
    uint8_t retries = 10;

    w5100_init(&ifconfig->ethconfig);
    
    //w5100_udp_bind(3, 12345);
    //w5100_set_endpoint(3, &debugaddr, 65530, NULL);
    w5100_udp_bind(2, 68);
    w5100_set_destipaddr(2, &gbcastipaddr);
    w5100_set_destport(2, 67);
    w5100_set_desthwaddr(2, &bcasthwaddr);


    do {
        _delay_ms(1000);
        dhcp_init_request(ifconfig, reqtype, elapsed);
	w5100_send(2, &dhcp_state.packet, dhcp_state.packetlen);
	//w5100_send(3, &dhcp_state.packet, dhcp_state.packetlen);
	
	while (w5100_udp_recv(2, &dhcp_state.udppacket, 1000)) {
	    //w5100_send(3, &dhcp_state.packet, dhcp_state.packetlen);

	    if (dhcp_state.packet.msgtype == 2) {
		if (!memcmp(&dhcp_state.packet.hwaddr, &ifconfig->ethconfig.hwaddr, 6)) {
#ifdef USE_DHCP
		    uint8_t *optptr = dhcp_state.packet.options;
		    uint8_t optcode;
		    uint8_t resptype = 0;

		    #define ldx() read_rx(x,r21)
		    #define ldx_str(v,len) copy_xz(x,v,len)

		    while ((optcode = *(optptr++)) != 255) {
			if (optcode != 0) {
			    uint8_t optlen = *(optptr++);
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
			if (memcmp(&dhcp_state.packet.yourip, "\0\0\0\0", 4)) {
			    optptr = (uint8_t *)&dhcp_state.packet.yourip;
			    ldx_str(&ifconfig->ethconfig.ipaddr, 4);
			    for (int i = 0; i < 4; i++) {
				ifconfig->bcastaddr.octet[i] = ifconfig->ethconfig.ipaddr.octet[i] | ((uint8_t)(~ifconfig->ethconfig.subnet.octet[i]));
			    }

			    if (reqtype == 1 && resptype == 2) {
				reqtype = 3;
				retries = 4;
				break;
			    } else if (resptype == 5) {
				return;
			    }
			}
		    } else if (resptype == 6) {
			reqtype = 1; 
			break;
		    }
#else
		    if (memcmp(&dhcp_state.packet.yourip, "\0\0\0\0", 4)) {
		        memcpy (&ifconfig->ethconfig.ipaddr, &dhcp_state.packet.yourip, 4);
			return;
                    } 
#endif
		}
	    }
	}

	elapsed += 1;
    } while (--retries);
}


#endif
