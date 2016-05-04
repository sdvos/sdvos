# Board Specific Configuration for Atmega328p breadboard
-include arch/avr5/config.mk

CFG += -DF_CPU=1000000UL
CFG += -DARCH_SRAM_END=0x08FF
CFG += -DKERN_STK_SIZE=0x0
CFG += -DIDLE_STK_SIZE=0x80
CFG += -DTERM_BAUD=9600

OBJ += board/ATMEGA328P/board.o

# Tool Chain Flags and Defs
CC = avr-gcc
LD = $(CC)
OBJDUMP = avr-objdump
# OBJDUMP_FLAGS = -d -j .sec1 -m avr5
OBJDUMP_FLAGS = -S
SIZE = avr-size
OBJCPY = avr-objcopy
OBJCPY_FLAGS = -O ihex
CFLAGS = -g -Os -std=c99 -Iinclude -I. -Wall -MMD -fno-inline-small-functions -fno-split-wide-types -mmcu=atmega328p $(CFG)
LDFLAGS = $(CFLAGS)
BIN = $(PROGRAM).hex
DIS = $(PROGRAM)_hex.dis

#Command to upload the binary using AVRISP MKII programmer
UPLOAD_COMMAND = avrdude -c avrispmkII -p atmega328p -P usb -U flash:w:$(BIN):i
