#include "config.h"
#include "tftp.h"
#include "w5100.h"
#include "util.h"

uint8_t tftp_open(struct ipaddr *ipaddr, char *filename) {
    w5100_udp_bind(2, 49152);
    w5100_set_destipaddr(2, ipaddr);
    w5100_set_destport(2, 69);
    
    if (ipaddr->octet[3] == 255) {
        w5100_set_desthwaddr(2, &bcasthwaddr);
    }

    for (int retries = 0; retries < 2; retries++) {
	tftp_state.packet.opcode[0] = 0;
	tftp_state.packet.opcode[1] = 1;

	char *buf = tftp_state.packet.filename_options;
	copy_zx_str_constz(buf, filename, 64);
	copy_zx_constz(buf, "octet\000blksize\000512\000", 18);

	w5100_send(2, &tftp_state.packet, (uint8_t *)buf - (uint8_t *)&tftp_state.packet);

	while (w5100_udp_recv(2, &tftp_state.udppacket, 500)) {
	    if (tftp_state.packet.opcode[0] == 0 && tftp_state.packet.opcode[1] == 6) {
		w5100_set_destipaddr(2, &tftp_state.serverip);
		w5100_set_destport(2, tftp_state.serverport);

		return tftp_read_block(0);
	    }
	}
    }

    return 0;
}

