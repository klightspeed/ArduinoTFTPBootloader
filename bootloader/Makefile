OBJS = main.o flash.o vectors.o
PRG = bootloader
LIBS = -letherten

all: $(PRG).elf lst text eeprom 

include ../avr.mk

LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_START)
CPPFLAGS += -DBOOTLOADER


