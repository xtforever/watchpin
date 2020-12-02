CC=gcc
CPPFLAGS+=-I/usr/local/include
CFLAGS+=-Os -s
CFLAGS+= -ffunction-sections -fdata-sections
LDFLAGS+= -Wl,--gc-sections
CFLAGS += -flto
LDFLAGS += -flto
LOADLIBES=-lwiringPi
all: watchpin

