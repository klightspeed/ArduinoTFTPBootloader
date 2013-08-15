#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000
#define DEFAULT_SPI_CLK_DIV 64
#define W5100_SPI_CLK_DIV 64
#define USEASMFUNC
#define DEFAULT_FW_FILENAME "EtherTen.bin"
#define DEFAULT_FW_FILENAME2 "\\Boot\\EtherTen.bin"
#define USE_DHCP
#define SDCARDS_PORTB (_BV(PB1))
#define SDCARDS_PORTD (_BV(PD4))
#define SEND_SD_IDLE

#endif /* CONFIG_H */
