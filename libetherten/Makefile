OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c)) $(patsubst %.S,%.o,$(wildcard src/*.S))
LIB = etherten
CPPFLAGS = -Iinclude

all: lib$(LIB).a

include avr.mk
