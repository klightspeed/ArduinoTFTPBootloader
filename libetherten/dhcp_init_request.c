#include "config.h"
#include <stdint.h>
#include <string.h>
#include "dhcp.h"
#include "util.h"

void dhcp_init_request(struct dhcp_ifconfig *ifconfig, uint8_t reqtype, uint8_t elapsed) {
    uint8_t *x = (uint8_t *)&dhcp_state;

    #define stx(v) write_rx(x,v,r21)
    #define stx_zrel(v) write_zx_rel(x,v,offsetof(struct dhcp_ifconfig, v))
    #define stx0() write_zero_x(x)
    #define stx0_str(len) zero_x(x,len)
    #define stx_str_const(v,len) copy_const_zx(x,v,len)
    #define stx_str(v,len) copy_zx(x,v,len)

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
    dhcp_state.packetlen = ((void *)x) - ((void *)&dhcp_state.packet);
#else
    stx0_str(offsetof(struct dhcp_state,packet.bootpopts) - offsetof(struct dhcp_state,packet.hwaddr_pad));
    stx0_str(64);
#endif
    dhcp_state.packetlen = sizeof(dhcp_state.packet);

    wdt_get_random_bytes(dhcp_state.packet.txid, 4);
}

