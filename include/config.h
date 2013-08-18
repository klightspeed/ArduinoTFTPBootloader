#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000
#define DEFAULT_SPI_CLK_DIV 64
#define W5100_SPI_CLK_DIV 64
#define USEASMFUNC
#define DEFAULT_FW_FILENAME "EtherTen.bin"
#define DEFAULT_FW_FILENAME2 "\\Boot\\EtherTen.bin"
#define SDCARDS_PORTB (_BV(PB1))
#define SDCARDS_PORTD (_BV(PD4))

#endif /* CONFIG_H */
