#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000
#define DEFAULT_SPI_CLK_DIV 64
#define W5100_SPI_CLK_DIV 64
#define USEASMFUNC
#define SDCARDS_PORTB (_BV(PB1))
#define SDCARDS_PORTD (_BV(PD4))
/* #define CONFIG_DHCP_RANDOM_XID */
/* #define CONFIG_RANDOM_HWADDR */
/* #define CONFIG_INIT_EEPROM_BOOTDATA */

#endif /* CONFIG_H */