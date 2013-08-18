#include "config.h"
#include <stdint.h>
#include <string.h>
#include "dhcp.h"
#include "util.h"

void __attribute__((naked)) __dhcp_init_request(void) {
    #define HEADERLEN (offsetof(struct dhcp_state,packet.elapsed))
    #define ZEROLEN (offsetof(struct dhcp_state,packet.dhcpcookie) - offsetof(struct dhcp_state,packet.elapsed))
    #define OPTRUN1LEN (3 + 3)
    #define OPTZEROLEN (6 + 6 + 6 + 18)
    #define OPTRUN2LEN (9 + 1)
    #define OPTSLEN (OPTRUN1LEN + OPTZEROLEN + OPTRUN2LEN)
    #define PACKETLEN (offsetof(struct dhcp_packet,options) + OPTSLEN)

    static const char header_opts[] PROGMEM = {
        0xFF, 0xFF, 0xFF, 0xFF, // Broadcast IP address
	0x43, 0x00, // Port 67
	(PACKETLEN & 0xFF), ((PACKETLEN >> 8) & 0xFF), // length
	0x01, // BOOTP request
	0x01, // HWADDR type 1
	0x06, // HWADDR length 6
	0x00, // Hops
	0x00, 0x00, 0x03, 0x31, // Transaction ID
	// Gap
        0x63, 0x82, 0x53, 0x63, // DHCP magic
	53, 1, 1, // DHCP request type
	61, 7, 1, /* 0, 0, 0, 0, 0, 0, */ // Client ID
	/* 50, 4, 0, 0, 0, 0, */ // Requested IP
	/* 54, 4, 0, 0, 0, 0, */ // Server IP
	/* 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, */ // Hostname
	55, 7, 1, 2, 6, 15, 42, 58, 59, // Requested options
	255 // End of options
    };

    asm volatile (
        "push r26\n\t"
	"push r27\n\t"
	"push r30\n\t"
	"push r31\n\t"
	// Copy fixed data
	"ldi r23, %[headerlen] + 1\n\t"
	"ldi r30, lo8(%[header_opts])\n\t"
	"ldi r31, hi8(%[header_opts])\n\t"
	"call __copy_const_zx\n\t"
	"ldi r23, %[zerolen] + 1\n\t"
	"call __zero_x\n\t"
	"ldi r23, %[optrun1len] + 5\n\t"
	"call __copy_const_zx\n\t"
	"ldi r23, %[optzerolen] + 1\n\t"
	"call __zero_x\n\t"
	"ldi r23, %[optrun2len] + 1\n\t"
	"call __copy_const_zx\n\t"
	// Fill hardware address
	"subi r26, lo8(%[packetlen] - %[hwaddrofs])\n\t"
	"sbci r27, hi8(%[packetlen] - %[hwaddrofs])\n\t"
	"pop r31\n\t"
	"pop r30\n\t"
	"ldi r23, 7\n\t"
	"adiw r30, %[ifhwaddrofs]\n\t"
	"call __copy_zx\n\t"
	// Fill client ID
	"subi r26, lo8(%[hwaddrofs] + 6 - (%[optsofs] + 2))\n\t"
	"sbci r27, hi8(%[hwaddrofs] + 6 - (%[optsofs] + 2))\n\t"
	"st X+, r20\n\t"
	"sbiw r30, 6\n\t"
	"adiw r26, 3\n\t"
	"ldi r23, 7\n\t"
	"call __copy_zx\n\t"
	// Only fill Requested IP and Server IP on DHCP Request
	"cpi r20, 3\n\t"
	"brne 1f\n\t"
	// Fill Requested IP
	"ldi r23, 50\n\t"
	"st X+, r23\n\t"
	"ldi r23, 4\n\t"
	"st X+, r23\n\t"
	"ldi r23, 5\n\t"
	"call __copy_zx\n\t"
	// Fill Server IP
	"ldi r23, 54\n\t"
	"st X+, r23\n\t"
	"ldi r23, 4\n\t"
	"st X+, r23\n\t"
	"ldi r23, 5\n\t"
	"call __copy_zx\n\t"
	"rjmp 2f\n\t"
	"1:\n\t"
	// Skip Requested IP and Server IP if not DHCP Request
	"adiw r26, 12\n\t"
	"adiw r30, 8\n\t"
	"2:\n\t"
	// Fill hostname
	"ld r23, Z\n\t"
	"breq 1f\n\t"
	"ldi r23, 12\n\t"
	"st X+, r23\n\t"
	"ldi r23, 16\n\t"
	"call __strlen_z\n\t"
	"subi r23, 1\n\t"
	"st X+, r23\n\t"
	"subi r23, -1\n\t"
	"call __copy_zx\n\t"
	"1:\n\t"
	"pop r27\n\t"
	"pop r26\n\t"
	"ret"
	:
	: [headerlen] "M" (HEADERLEN),
	  [header_opts] "i" (header_opts),
	  [zerolen] "M" (ZEROLEN),
	  [optslen] "M" (OPTSLEN),
	  [optrun1len] "M" (OPTRUN1LEN),
	  [optzerolen] "M" (OPTZEROLEN),
	  [optrun2len] "M" (OPTRUN2LEN),
	  [packetlen] "i" (PACKETLEN),
	  [hwaddrofs] "i" (offsetof(struct dhcp_packet,hwaddr)),
	  [ifhwaddrofs] "I" (offsetof(struct dhcp_ifconfig,ethconfig.hwaddr)),
	  [optsofs] "i" (offsetof(struct dhcp_packet,options))
    );
}

