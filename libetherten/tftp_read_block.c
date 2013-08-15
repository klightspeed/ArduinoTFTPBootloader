#include "config.h"
#include <string.h>
#include "tftp.h"
#include "w5100.h"

uint8_t tftp_read_block(uint16_t blknum) {
    union twobyte __blknum;
    union twobyte respblknum;
    __blknum.word = blknum;
    respblknum.word = blknum + 1;
    for (int retries = 0; retries < 5; retries++) {
	tftp_state.packet.opcode[0] = 0;
	tftp_state.packet.opcode[1] = 4;
	tftp_state.packet.blknum[0] = __blknum.byte[1];
	tftp_state.packet.blknum[1] = __blknum.byte[0];

	w5100_send(2, &tftp_state.packet, 4);

	memset (&tftp_state, 0, sizeof(tftp_state));
	
	while (w5100_udp_recv(2, &tftp_state.udppacket, 1000)) {
	    if (tftp_state.packet.opcode[0] == 0 && 
	        tftp_state.packet.opcode[1] == 3 &&
		tftp_state.packet.blknum[0] == respblknum.byte[1] && 
		tftp_state.packet.blknum[1] == respblknum.byte[0]) {
		
		if (tftp_state.packetlen == 4) {
		    uint8_t buf[4] = { 0, 4, respblknum.byte[1], respblknum.byte[0] };
		    w5100_send(2, buf, 4);
		    w5100_sock_close(2);
		    return 0;
		}

		return 1;
	    }
	}
    }

    return 0;
}


