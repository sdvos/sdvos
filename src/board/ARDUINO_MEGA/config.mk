# Board Specific Configuration for Arduino MEGA
-include arch/avr6/config.mk

CFG += -DF_CPU=16000000UL
CFG += -DARCH_SRAM_END=0x21FF
CFG += -DKERN_STK_SIZE=0x0
CFG += -DIDLE_STK_SIZE=0x80

OBJ += board/ARDUINO_MEGA/board.o

# Tool Chain Flags and Defs
CC = avr-gcc
LD = $(CC)
OBJDUMP = avr-objdump
# OBJDUMP_FLAGS = -d -j .sec1 -m avr6
OBJDUMP_FLAGS = -S
SIZE = avr-size
OBJCPY = avr-objcopy
OBJCPY_FLAGS = -O ihex
CFLAGS = -g -Os -std=c99 -Iinclude -I. -Wall -MMD -fno-inline-small-functions -fno-split-wide-types -mmcu=atmega2560 $(CFG)
LDFLAGS = $(CFLAGS)
BIN = $(PROGRAM).hex
DIS = $(PROGRAM)_hex.dis

# Command to upload the binary to the board
UPLOAD_COMMAND = avrdude -c wiring -p atmega2560 -P /dev/ttyACM0 -b 115200 -F -D -U flash:w:$(BIN):i
