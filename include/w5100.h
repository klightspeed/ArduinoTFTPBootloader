#ifndef W5100_H
#define W5100_H

#include "config.h"
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "spi.h"
#include "util.h"

#define W5100_PORT     PORTB
#define W5100_DDR      DDRB
#define W5100_PIN      PINB
#define W5100_SCK_BIT  SPI_SPI0_SCK_BIT
#define W5100_MISO_BIT SPI_SPI0_MISO_BIT
#define W5100_MOSI_BIT SPI_SPI0_MOSI_BIT
#define W5100_SS_BIT   PORTB2

#define W5100_OP_READ  0x0F
#define W5100_OP_WRITE 0xF0

#define W5100_OP_READ_RAM_BIT 0
#define W5100_OP_WRITE_RAM_BIT 1
#define W5100_OP_WRITE_FLASH_BIT 2
#define W5100_OP_CMD_BIT 5

#define W5100_OP_READ_RAM (W5100_OP_READ | _BV(W5100_OP_READ_RAM_BIT + 8))
#define W5100_OP_WRITE_RAM (W5100_OP_WRITE | _BV(W5100_OP_WRITE_RAM_BIT + 8))
#define W5100_OP_WRITE_FLASH (W5100_OP_WRITE | _BV(W5100_OP_WRITE_FLASH_BIT + 8))
#define W5100_OP_RECV_RAM (W5100_OP_READ_RAM | _BV(W5100_OP_CMD_BIT + 8))
#define W5100_OP_SEND_RAM (W5100_OP_WRITE_RAM | _BV(W5100_OP_CMD_BIT + 8))
#define W5100_OP_SEND_FLASH (W5100_OP_WRITE_FLASH | _BV(W5100_OP_CMD_BIT + 8))

#define W5100_BUFSIZE_1k        0x00
#define W5100_BUFSIZE_2k        0x01
#define W5100_BUFSIZE_4k        0x02
#define W5100_BUFSIZE_8k        0x03

#define W5100_BUFSIZE(s0,s1,s2,s3) \
    ((W5100_BUFSIZE_ ## s0) << 0) | \
    ((W5100_BUFSIZE_ ## s1) << 2) | \
    ((W5100_BUFSIZE_ ## s2) << 4) | \
    ((W5100_BUFSIZE_ ## s3) << 6)

#define W5100_MR_CLOSED         0x00
#define W5100_MR_TCP            0x01
#define W5100_MR_UDP            0x02
#define W5100_MR_IPRAW          0x03
#define W5100_MR_MACRAW         0x04
#define W5100_MR_PPPOE          0x05

#define W5100_CR_OPEN           0x01
#define W5100_CR_LISTEN         0x02
#define W5100_CR_CONNECT        0x04
#define W5100_CR_DISCON         0x08
#define W5100_CR_CLOSE          0x10
#define W5100_CR_SEND           0x20
#define W5100_CR_SEND_MAC       0x21
#define W5100_CR_SEND_KEEP      0x22
#define W5100_CR_RECV           0x40

#define W5100_SOCK_CLOSED       0x00
#define W5100_SOCK_ARP1         0x11
#define W5100_SOCK_INIT         0x13
#define W5100_SOCK_LISTEN       0x14
#define W5100_SOCK_SYNSENT      0x15
#define W5100_SOCK_SYNRECV      0x16
#define W5100_SOCK_ESTABLISHED  0x17
#define W5100_SOCK_FIN_WAIT     0x18
#define W5100_SOCK_CLOSING      0x1A
#define W5100_SOCK_TIME_WAIT    0x1B
#define W5100_SOCK_CLOSE_WAIT   0x1C
#define W5100_SOCK_LAST_ACK     0x1D
#define W5100_SOCK_ARP2         0x21
#define W5100_SOCK_UDP          0x22
#define W5100_SOCK_ARP3         0x31
#define W5100_SOCK_IPRAW        0x32
#define W5100_SOCK_MACRAW       0x42
#define W5100_SOCK_PPPOE        0x5F

#ifdef __ASSEMBLER__
#define w5100_spi_read spi_spi0_read
#define w5100_spi_write spi_spi0_write
#else

#include <stdint.h>
#include <stddef.h>

struct ipaddr {
    uint8_t octet[4];
};

struct hwaddr {
    uint8_t octet[6];
};

struct ipendpoint {
    struct ipaddr ipaddr;
    uint16_t port;
};

struct __attribute__((__packed__)) w5100_ifconfig {
    /* 0x0001 */ struct ipaddr gateway;
    /* 0x0005 */ struct ipaddr subnet;
    /* 0x0009 */ struct hwaddr hwaddr;
    /* 0x000F */ struct ipaddr ipaddr;
};

struct __attribute__((__packed__)) w5100_regs_base {
    /* 0x0000 */ uint8_t mode;
    /* 0x0001 */ struct w5100_ifconfig ifconfig;
    /* 0x0013 */ uint8_t reserved1[2];
    /* 0x0015 */ uint8_t intreg;
    /* 0x0016 */ uint8_t intmask;
    /* 0x0017 */ uint8_t retrytime[2];
    /* 0x0019 */ uint8_t retrycount;
    /* 0x001A */ uint8_t rxmemsize;
    /* 0x001B */ uint8_t txmemsize;
};

struct __attribute__((__packed__)) w5100_regs_common {
    /* 0x0000 */ uint8_t mode;
    /* 0x0001 */ struct ipaddr gateway;
    /* 0x0005 */ struct ipaddr subnet;
    /* 0x0009 */ struct hwaddr hwaddr;
    /* 0x000F */ struct ipaddr ipaddr;
    /* 0x0013 */ uint8_t reserved1[2];
    /* 0x0015 */ uint8_t intreg;
    /* 0x0016 */ uint8_t intmask;
    /* 0x0017 */ uint16_t retrytime;
    /* 0x0019 */ uint8_t retrycount;
    /* 0x001A */ uint8_t rxmemsize;
    /* 0x001B */ uint8_t txmemsize;
    /* 0x001C */ uint8_t reserved2[0x002A - 0x001C];
    /* 0x002A */ struct ipaddr unreachip;
    /* 0x002E */ uint16_t unreachport;
    /* 0x0030 */ uint8_t reserved3[0x0400 - 0x0030];
};

struct __attribute__((__packed__)) w5100_regs_socket {
    /* 0x0000 */ uint8_t mode;
    /* 0x0001 */ uint8_t command;
    /* 0x0002 */ uint8_t intreg;
    /* 0x0003 */ uint8_t status;
    /* 0x0004 */ uint16_t srcport;
    /* 0x0006 */ struct hwaddr dsthwaddr;
    /* 0x000C */ struct ipaddr dstipaddr;
    /* 0x0010 */ uint16_t dstport;
    /* 0x0012 */ uint16_t maxsegsize;
    /* 0x0014 */ uint8_t rawproto;
    /* 0x0015 */ uint8_t tos;
    /* 0x0016 */ uint8_t ttl;
    /* 0x0017 */ uint8_t reserved1[0x0020 - 0x0017];
    /* 0x0020 */ uint16_t txfree;
    /* 0x0022 */ uint16_t txrdptr;
    /* 0x0024 */ uint16_t txwrptr;
    /* 0x0026 */ uint16_t rxrcvd;
    /* 0x0028 */ uint16_t rxrdptr;
    /* 0x002A */ uint8_t reserved2[0x0100 - 0x002A];
};

struct __attribute__((__packed__)) w5100_regs {
    struct w5100_regs_common common;
    struct w5100_regs_socket socket[4];
    uint8_t reserved1[0x4000 - 0x0800];
    uint8_t txbuffer[0x2000];
    uint8_t rxbuffer[0x2000];
};

struct w5100_sock_bufptr {
    uint16_t base;
    uint16_t mask;
    uint16_t regbase;
    uint16_t memptr;
    uint16_t avail;
};

struct w5100_sock_ptrs {
    struct w5100_sock_bufptr rxbuf;
    struct w5100_sock_bufptr txbuf;
};

struct w5100_udp_packet {
    struct ipaddr ipaddr;
    uint16_t port;
    uint16_t len;
    uint8_t data[];
};

struct w5100_sock_reg {
    uint8_t socknum;
    uint8_t offset;
};

#define RXBUFBASE(s) MAKEWORD(0,0x60 | ((s) * 8))
#define TXBUFBASE(s) MAKEWORD(0,0x40 | ((s) * 8))

#define W5100_PORT_SET(b) W5100_PORT |= _BV(W5100_ ## b ## _BIT)
#define W5100_PORT_CLR(b) W5100_PORT &= ~_BV(W5100_ ## b ## _BIT)
#define W5100_DDR_OUT(b)  W5100_DDR  |= _BV(W5100_ ## b ## _BIT)
#define W5100_DDR_IN(b)   W5100_DDR  &= ~_BV(W5100_ ## b ## _BIT)
#define W5100_PIN_SET(b)  (W5100_PIN & _BV(W5100_ ## b ## _BIT)

#define w5100_select()    W5100_PORT_CLR(SS)
#define w5100_deselect()  W5100_PORT_SET(SS)

#define w5100_spi_datareg spi_spi0_datareg
#define w5100_spi_write(v) spi_spi0_write(v)
#define w5100_spi_wait() spi_spi0_wait()
#define w5100_spi_read() spi_spi0_read()
#define w5100_spi_setup(div) spi_spi0_setup(div)

#define W5100_COMMON_REGADDR(r) MAKEWORD(offsetof(struct w5100_regs,r),0)
#define W5100_SOCK_REGBASE(s)   MAKEWORD(0,(s) | 4)
#define W5100_SOCK_REGADDR(s,r) MAKEWORD(offsetof(struct w5100_regs_socket,r),(s) | 4)

extern struct hwaddr bcasthwaddr;
extern struct ipaddr gbcastipaddr;
extern struct w5100_regs_base w5100_reginit;

void w5100_init(struct w5100_ifconfig *ifconfig);
void ___w5100_write_byte(uint16_t address, uint8_t value, uint8_t op);
void __w5100_rw_mem(uint16_t address, void *value, int len, uint16_t optype);
void __w5100_sock_rw_mem(uint8_t socknum, void *buf, int len, uint16_t optype, int offset);
void __w5100_sock_cmd(uint8_t socknum, uint8_t cmd);

#define __w5100_write_byte(address,value,op) \
    { \
	register uint16_t __address asm("r24") = (address); \
	register uint8_t __value asm("r22") = (value); \
	register uint8_t __op asm("r20") = (op); \
	asm volatile ("call ___w5100_write_byte" : : "w" (__address), "r" (__value), "r" (__op)); \
    } while (0)

#define __w5100_rw_byte(address,value,op) \
    ({ \
        __w5100_write_byte(address, value, op); \
	(op == W5100_OP_READ) ? w5100_spi_read() : 0; \
    })

#define w5100_write_byte(address,value) __w5100_rw_byte(address, value, W5100_OP_WRITE)
#define w5100_read_byte(address) __w5100_rw_byte(address, 0xFF, W5100_OP_READ)

#define w5100_write_word(address,value) \
    do { \
        register uint16_t __address asm("r24") = address; \
	register uint8_t __vallo asm("r22") = LO8(value); \
	register uint8_t __op asm("r20") = W5100_OP_WRITE; \
	uint8_t __valhi = HI8(value); \
	asm volatile ( \
	    "adiw r24, 1\n\t" \
	    "call ___w5100_write_byte\n\t" \
	    "sbiw r24, 1\n\t" \
	    "mov %[vallo], %[valhi]\n\t" \
	    "call ___w5100_write_byte\n\t" \
	    : [vallo] "+r" (__vallo) \
	    : "w" (__address), [valhi] "r" (__valhi), "r" (__op) \
	); \
    } while (0)

#define w5100_read_word(address) \
    ({ \
        union twobyte __value; \
        register uint16_t __address asm("r24") = address; \
	register uint8_t __vallo asm("r22") = 0xFF; \
	register uint8_t __op asm("r20") = W5100_OP_READ; \
	asm volatile ( \
	    "adiw r24, 1\n\t" \
	    "call ___w5100_write_byte\n\t" \
	    "sbiw r24, 1\n\t" \
	    : : "w" (__address), "r" (__vallo), "r" (__op) \
	); \
	__value.byte[0] = w5100_spi_read(); \
	__vallo = 0xFF; \
	asm volatile ( \
	    "call ___w5100_write_byte\n\t" \
	    : : "w" (__address), "r" (__vallo), "r" (__op) \
	); \
	__value.byte[1] = w5100_spi_read(); \
	__value.word; \
    })

#define w5100_write_mem(address,value,len) __w5100_rw_mem(address, value, len, W5100_OP_WRITE_RAM)
#define w5100_write_mem_p(address,value,len) __w5100_rw_mem(address, value, len, W5100_OP_WRITE_FLASH)
#define w5100_read_mem(address,value,len) __w5100_rw_mem(address, value, len, W5100_OP_READ_RAM)
#define w5100_sock_write(socknum,value,len,offset) __w5100_sock_rw_mem(socknum, value, len, W5100_OP_WRITE_RAM, offset)
#define w5100_sock_write_p(socknum,value,len,offset) __w5100_sock_rw_mem(socknum, value, len, W5100_OP_WRITE_FLASH, offset)
#define w5100_sock_read(socknum,value,len,offset) __w5100_sock_rw_mem(socknum, value, len, W5100_OP_READ_RAM, offset)
#define w5100_sock_rxavail(socknum) w5100_read_word(W5100_SOCK_REGADDR(socknum, rxrcvd))
#define w5100_sock_txfree(socknum) w5100_read_word(W5100_SOCK_REGADDR(socknum, txfree))
#define w5100_send(socknum,data,len,offset) __w5100_sock_rw_mem(socknum, data, len, W5100_OP_SEND_RAM, offset)
#define w5100_send_p(socknum,data,len,offset) __w5100_sock_rw_mem(socknum, data, len, W5100_OP_SEND_FLASH, offset)

#define w5100_sock_cmd(socknum,cmd) \
    { \
	register uint8_t __socknum asm("r24") = (socknum); \
	register uint8_t __cmd asm("r22") = (cmd); \
	asm volatile ( "call __w5100_sock_cmd" : "+r" (__socknum), "+r" (__cmd) : : "r25", "r20", "r0"); \
    } while (0)

#define w5100_sock_close(socknum) w5100_sock_cmd(socknum, W5100_CR_CLOSE)

#define w5100_write_reg_byte(r,v) w5100_write_byte(W5100_COMMON_REGADDR(r),v)
#define w5100_write_reg_word(r,v) w5100_write_word(W5100_COMMON_REGADDR(r),v)
#define w5100_write_reg_mem(r,v,l) w5100_write_mem(W5100_COMMON_REGADDR(r),v,l)
#define w5100_write_reg_mem_p(r,v,l) w5100_write_mem_p(W5100_COMMON_REGADDR(r),v,l)
#define w5100_read_reg_byte(r) w5100_read_byte(W5100_COMMON_REGADDR(r))
#define w5100_read_reg_word(r) w5100_read_word(W5100_COMMON_REGADDR(r))
#define w5100_read_reg_mem(r,v,l) w5100_read_mem(W5100_COMMON_REGADDR(r),v,l)

#define w5100_sock_write_reg_byte(s,r,v) w5100_write_byte(W5100_SOCK_REGADDR(s,r),v)
#define w5100_sock_write_reg_word(s,r,v) w5100_write_word(W5100_SOCK_REGADDR(s,r),v)
#define w5100_sock_write_reg_mem(s,r,v,l) w5100_write_mem(W5100_SOCK_REGADDR(s,r),v,l)
#define w5100_sock_write_reg_mem_p(s,r,v,l) w5100_write_mem_p(W5100_SOCK_REGADDR(s,r),v,l)
#define w5100_sock_read_reg_byte(s,r) w5100_read_byte(W5100_SOCK_REGADDR(s,r))
#define w5100_sock_read_reg_word(s,r) w5100_read_word(W5100_SOCK_REGADDR(s,r))
#define w5100_sock_read_reg_mem(s,r,v,l) w5100_read_mem(W5100_SOCK_REGADDR(s,r),v,l)
#define w5100_set_destipaddr(socknum,ipaddr) w5100_write_mem(W5100_SOCK_REGADDR(socknum, dstipaddr), ipaddr, 4)
#define w5100_set_destport(socknum,port) w5100_write_word(W5100_SOCK_REGADDR(socknum, dstport), port)
#define w5100_set_desthwaddr(socknum,hwaddr) w5100_write_mem(W5100_SOCK_REGADDR(socknum, dsthwaddr), hwaddr, 6)

#define w5100_udp_read(socknum,buf,len,offset) w5100_sock_read(socknum, buf, len, (offset) + 8)

static inline int w5100_udp_peek(uint8_t socknum, struct w5100_udp_packet *packet) {
    if (w5100_sock_rxavail(socknum) != 0) {
        w5100_sock_read(socknum, packet, 8, 0);
        BYTESWAP(&packet->port);
        BYTESWAP(&packet->len);
        return 1;
    }

    return 0;
}

static inline int w5100_udp_try_recv(uint8_t socknum, struct w5100_udp_packet *packet) {
    if (w5100_sock_rxavail(socknum) != 0) {
	__w5100_sock_rw_mem(socknum, packet, 8, W5100_OP_RECV_RAM, 0);
	BYTESWAP(&packet->port);
	BYTESWAP(&packet->len);
	__w5100_sock_rw_mem(socknum, packet->data, packet->len, W5100_OP_RECV_RAM, 0);
	return 1;
    }

    return 0;
}

static inline void w5100_udp_end_recv(uint8_t socknum, struct w5100_udp_packet *packet) {
    if (w5100_sock_rxavail(socknum) != 0) {
	__w5100_sock_rw_mem(socknum, packet, 8, W5100_OP_RECV_RAM, 0);
	BYTESWAP(&packet->port);
	BYTESWAP(&packet->len);
	__w5100_sock_rw_mem(socknum, packet->data, 0, W5100_OP_RECV_RAM, packet->len);
    }
}

static inline int w5100_udp_recv(uint8_t socknum, struct w5100_udp_packet *packet, int timeout) {
    while (timeout--) {
        if (w5100_udp_try_recv(socknum, packet)) return 1;
	_delay_ms(1);
    }

    return 0;
}

static inline int w5100_udp_wait(uint8_t socknum, struct w5100_udp_packet *packet, int timeout) {
    while (timeout--) {
        if (w5100_sock_rxavail(socknum) != 0) {
	    w5100_udp_peek(socknum, packet);
	    return 1;
	}
	_delay_ms(1);
    }

    return 0;
}

static inline void w5100_udp_bind(uint8_t socknum, uint16_t port) {
    while (1) {
        w5100_write_byte(W5100_SOCK_REGADDR(socknum, mode), W5100_MR_UDP);
	w5100_write_word(W5100_SOCK_REGADDR(socknum, srcport), port);
	w5100_sock_cmd(socknum, W5100_CR_OPEN);

	if (w5100_read_byte(W5100_SOCK_REGADDR(socknum, status)) == W5100_SOCK_UDP) {
	    break;
	} else {
	    w5100_sock_cmd(socknum, W5100_CR_CLOSE);
	}
    }
}

static inline int w5100_tcp_try_recv(uint8_t socknum, uint8_t *buf, int maxread, int minread) {
    int rxavail = w5100_sock_rxavail(socknum);

    if (rxavail >= (minread ?: 1)) {
        if (rxavail > maxread) {
            rxavail = maxread;
        }

        __w5100_sock_rw_mem(socknum, buf, rxavail, W5100_OP_RECV_RAM, 0);
        return rxavail;
    }

    return 0;
}

static inline int w5100_tcp_recv(uint8_t socknum, uint8_t *buf, int maxread, int minread, int timeout) {
    while (timeout--) {
        int len = w5100_tcp_try_recv(socknum, buf, maxread, minread);
        if (len) return len;
        _delay_ms(1);
    }

    return w5100_tcp_try_recv(socknum, buf, maxread, 1);
}

#endif /* __ASSEMBLER__ */

#endif /* W5100_H */
