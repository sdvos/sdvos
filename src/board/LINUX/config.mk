# Board Specific Configuration for Linux
CFG += -D__ARCH_LINUX__
# SRAM_END is only used to calculate offsets
CFG += -DARCH_SRAM_END=0x20000000
CFG += -DKERN_STK_SIZE=0x0
CFG += -DIDLE_STK_SIZE=0x1000

# Objects specific for Arduino UNO
OBJ += arch/linux/task.o
OBJ += arch/linux/panic.o
OBJ += arch/linux/interrupt.o
OBJ += arch/linux/syscall.o
OBJ += arch/linux/timer.o
OBJ += arch/linux/idle.o
OBJ += arch/linux/utils.o
OBJ += arch/linux/mcu.o
#OBJ += drivers/usart/linux_usart.o
OBJ += board/LINUX/board.o

# Tool Chain Flags and Defs
CC = gcc
LD = $(CC)
LIBS = -lrt
OBJDUMP = objdump
OBJDUMP_FLAGS = -S
SIZE = size
OBJCPY = objcopy
OBJCPY_FLAGS = -O ihex
CFLAGS = -g -Os -std=gnu99 -m32 -Iinclude -I. -Wall -MMD $(CFG)
LDFLAGS = $(CFLAGS)
BIN = $(PROGRAM).hex
DIS = $(PROGRAM)_hex.dis

# Command to upload the binary to the board
UPLOAD_COMMAND = ./sdvos

