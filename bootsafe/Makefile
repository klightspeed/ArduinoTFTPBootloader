OBJS = vectors.o flash.o wdt_reset.o multiboot.o bootloader-stubs.o compare_const_zx.o
PRG = bootsafe

all: $(PRG).elf lst text eeprom 

include ../avr.mk

LDFLAGS += -ffreestanding -nostartfiles -nodefaultlibs -Wl,--section-start=.text=$(BOOTSAFE_START)
