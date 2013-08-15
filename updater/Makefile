OBJS = main.o bootloader.o
PRG = updater
LIBS = -letherten

all: $(PRG).elf lst text eeprom 

../bootloader/bootloader.bin:
	$(MAKE) -C ../bootloader bootloader.bin

bootloader.o: ../bootloader/bootloader.bin	

include ../avr.mk
