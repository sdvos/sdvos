# Board Specific Configuration for Arduino UNO
CFG += -D__ARCH_AVR5__
CFG += -DF_CPU=16000000UL
CFG += -DARCH_SRAM_END=0x08FF
CFG += -DKERN_STK_SIZE=0x0
CFG += -DIDLE_STK_SIZE=0x80
# Page size for AVR5. If not set, default to 128 bytes.
#CFG += -DPGSIZE=0x80
#CFG += -DWITH_FLASH_UTILITY

# Objects specific for Arduino UNO
OBJ += arch/avr5/task.o
OBJ += arch/avr5/panic.o
OBJ += arch/avr5/interrupt.o
OBJ += arch/avr5/syscall.o
OBJ += arch/avr5/timer.o
OBJ += arch/avr5/idle.o
OBJ += arch/avr5/utils.o
OBJ += arch/avr5/atomic.o
#OBJ += drivers/usart/atmega328p_usart.o
#OBJ += arch/avr5/pgm_mem.o
OBJ += board/ARDUINO_UNO/board.o

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

# Command to upload the binary to the board
UPLOAD_COMMAND = avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U flash:w:$(BIN):i

