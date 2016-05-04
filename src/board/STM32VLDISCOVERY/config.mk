# Board Specific Configuration for STM32VLDISCOVERY
-include arch/armv7m/config.mk

CFG += -DARCH_SRAM_END=0x20002000
CFG += -DKERN_STK_SIZE=0x100
CFG += -DIDLE_STK_SIZE=0x100

# Objects specific for STM32VLDISCOVERY
# OBJ += drivers/uart/stm32f10x_uart.o
OBJ += board/STM32VLDISCOVERY/board.o

# Tool Chain Flags and Defs
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJDUMP = arm-none-eabi-objdump
OBJDUMP_FLAGS = -S
SIZE = arm-none-eabi-size
OBJCPY = arm-none-eabi-objcopy
OBJCPY_FLAGS = -Obinary
CFLAGS = -g -Os -std=c99 -Iinclude -I. -Wall -MMD -fno-common \
         -mcpu=cortex-m3 -mthumb -mlittle-endian $(CFG)
LDFLAGS = -Tboard/STM32VLDISCOVERY/sdvos.ld -nostartfiles
BIN = $(PROGRAM).bin
DIS = $(PROGRAM).dis

# Command to upload the binary to the board
UPLOAD_COMMAND = st-flash write /dev/sg0 $(BIN) 0x8000000

