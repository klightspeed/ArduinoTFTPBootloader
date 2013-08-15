AVR_MK_SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
DEBUG = no
BOOTLOADER = yes

APP_START         = 0x00000000
BOOTLOADER_START  = 0x00007000
BOOTSAFE_START    = 0x00007C00
BOOTFUNCS_START   = 0x00007C80
OPTIBOOT_START    = 0x00007E00
MCUSRMIRROR_ADDR  = 0x008008FF
STACKEND_ADDR     = 0x008008FD
EEPROM_DATA_START = 0x00810380

MCU = atmega328p

CC = avr-gcc
AS = avr-as
LD = avr-ld
AR = avr-ar
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

CFLAGS = -mmcu=$(MCU) -Os -fomit-frame-pointer -Wall --std=gnu99
ASFLAGS = -mmcu=$(MCU)
LDFLAGS = -mrelax -mmcu=$(MCU) -L$(AVR_MK_SELF_DIR)/libetherten
CPPFLAGS = -I$(AVR_MK_SELF_DIR)/include
OBJCOPYTEXT = -j .text -j .data

LDFLAGS += -Wl,--section-start=.application=$(APP_START) \
           -Wl,--section-start=.bootsafe=$(BOOTSAFE_START) \
           -Wl,--section-start=.bootloader=$(BOOTLOADER_START) \
	   -Wl,--section-start=.bootfuncs=$(BOOTFUNCS_START) \
	   -Wl,--section-start=.optiboot=$(OPTIBOOT_START) \
	   -Wl,--section-start=.eeprom.boot=$(EEPROM_DATA_START) \
	   -Wl,--defsym=__mcusr_mirror=$(MCUSRMIRROR_ADDR) \
	   -Wl,--defsym=__stack=$(STACKEND_ADDR)

ifeq ($(DEBUG), yes)
CPPFLAGS += -DDEBUG
endif

CLEAN_FILES = *.o *.a *.elf *.eps *.png *.pdf *.bak *.lst *.map *.hex *.bin *.srec

.PHONY: lst text eeprom hex bin srec ehex ebin esrec dox eps png pdf

$(PRG).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

lib$(LIB).a: $(OBJS)
	$(AR) crs $@ $^

clean:
	rm -f $(CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) $(OBJCOPYTEXT) -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) $(OBJCOPYTEXT) -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) $(OBJCOPYTEXT) -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = 

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@


