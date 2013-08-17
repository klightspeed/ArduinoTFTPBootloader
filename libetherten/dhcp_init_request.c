#include "config.h"
#include <stdint.h>
#include <string.h>
#include "dhcp.h"
#include "util.h"

void __attribute__((naked)) __dhcp_init_request(void) {
    #define HEADERLEN (offsetof(struct dhcp_state,packet.hops))
    #define ZEROLEN (offsetof(struct dhcp_state,packet.dhcpcookie) - offsetof(struct dhcp_state,packet.hops))
    #define OPTSLEN (3 + 9 + 6 + 6 + 6 + 1)
    #define PACKETLEN (offsetof(struct dhcp_packet,options) + OPTSLEN)

    static const char header_opts[] PROGMEM = {
        0xFF, 0xFF, 0xFF, 0xFF, // Broadcast IP address
	0x43, 0x00, // Port 67
	(PACKETLEN & 0xFF), ((PACKETLEN >> 8) & 0xFF), // length
	0x01, // BOOTP request
	0x01, // HWADDR type 1
	0x06, // HWADDR length 6
	// Gap
        0x63, 0x82, 0x53, 0x63, // DHCP magic
	53, 1, 1, // DHCP request type
	61, 7, 1, 0, 0, 0, 0, 0, 0, // Client ID
	50, 4, 0, 0, 0, 0, // Requested IP
	0, 0, 0, 0, 0, 0, // Server IP
	55, 4, 1, 2, 6, 42, // Requested options
	255 // End of options
    };

    asm volatile (
        "push r26\n\t"
	"push r27\n\t"
	"push r30\n\t"
	"push r31\n\t"
	"ldi r23, %[headerlen] + 1\n\t"
	"ldi r30, lo8(%[header_opts])\n\t"
	"ldi r31, hi8(%[header_opts])\n\t"
	"call __copy_const_zx\n\t"
	"ldi r23, %[zerolen] + 1\n\t"
	"call __zero_x\n\t"
	"ldi r23, %[optslen] + 5\n\t"
	"call __copy_const_zx\n\t"
	"subi r26, lo8(%[packetlen] - %[hwaddrofs])\n\t"
	"sbci r27, hi8(%[packetlen] - %[hwaddrofs])\n\t"
	"pop r31\n\t"
	"pop r30\n\t"
	"ldi r23, 7\n\t"
	"adiw r30, %[ifhwaddrofs]\n\t"
	"call __copy_zx\n\t"
	"subi r26, lo8(%[hwaddrofs] + 6 - (%[optsofs] + 2))\n\t"
	"sbci r27, hi8(%[hwaddrofs] + 6 - (%[optsofs] + 2))\n\t"
	"st X+, r20\n\t"
	"sbiw r30, 6\n\t"
	"adiw r26, 3\n\t"
	"ldi r23, 7\n\t"
	"call __copy_zx\n\t"
	"adiw r26, 2\n\t"
	"ldi r23, 5\n\t"
	"call __copy_zx\n\t"
	"cpi r20, 3\n\t"
	"brne 1f\n\t"
	"ldi r23, 54\n\t"
	"st X+, r23\n\t"
	"ldi r23, 4\n\t"
	"st X+, r23\n\t"
	"ldi r23, 5\n\t"
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
	  [packetlen] "i" (PACKETLEN),
	  [hwaddrofs] "i" (offsetof(struct dhcp_packet,hwaddr)),
	  [ifhwaddrofs] "I" (offsetof(struct dhcp_ifconfig,ethconfig.hwaddr)),
	  [optsofs] "i" (offsetof(struct dhcp_packet,options))
    );

#if 0
    register uint16_t packetlen asm("r24");

    asm volatile ("" : "=x" (x), "=r" (ifconfig), "=r" (reqtype), "=r" (elapsed));
    
    #define stx(v) write_rx(x,v,r21)
    #define stx_zrel(v) write_zx_rel(x,v,offsetof(struct dhcp_ifconfig, v))
    #define stx0() write_zero_x(x)
    #define stx0_str(len) zero_x(x,len)
    #define stx_str_const(v,len) copy_const_zx(x,v,len)
    #define stx_str(v,len) copy_zx(x,v,len)

    asm volatile ("push r24\n\tpush r25");
    asm volatile ("push r26\n\tpush r27");
    stx_str_const("\xFF\xFF\xFF\xFF\x43\x00\x00\x00\x01\x01\x06\x00\00\00\00\00\00", 17);
    stx(elapsed);
    stx0_str(offsetof(struct dhcp_state,packet.hwaddr) - offsetof(struct dhcp_state,packet.flags));
    stx_str(&ifconfig->ethconfig.hwaddr, 6);
#ifdef USE_DHCP
    stx0_str(offsetof(struct dhcp_state,packet.dhcpcookie) - offsetof(struct dhcp_state,packet.hwaddr_pad));
    stx_str_const("\x63\x82\x53\x63\x35\x01", 6);
    stx(reqtype);
    stx(61);
    stx(7);
    stx(1);
    stx_str(&ifconfig->ethconfig.hwaddr, 6);
    stx(50);
    stx(4);
    stx_str(&ifconfig->ethconfig.ipaddr, 4);
    if (reqtype == 3) {
	stx(54);
	stx(4);
	stx_str(&ifconfig->dhcpserver, 4);
    }
    stx_str_const("\x37\x04\x01\x03\x06\x2A\xFF", 7);
#else
    stx0_str(offsetof(struct dhcp_state,packet.bootpopts) - offsetof(struct dhcp_state,packet.hwaddr_pad));
    stx0_str(64);
#endif
    packetlen = (uint16_t)x;
    asm volatile ("pop r27\n\tpop r26" : "=x" (x));
    packetlen -= (uint16_t)x;
    packetlen -= offsetof(struct dhcp_state,packet);
    x += offsetof(struct dhcp_state,packetlen);
    stx(LO8(packetlen));
    stx(HI8(packetlen));
    x -= offsetof(struct dhcp_state,packetlen) + 2;
#endif
}

