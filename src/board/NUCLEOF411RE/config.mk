# Board Specific Configuration for NUCLEOF411RE
-include arch/armv7m/config.mk

CFG += -DARCH_SRAM_END=0x20020000
CFG += -DKERN_STK_SIZE=0x100
CFG += -DIDLE_STK_SIZE=0x100
# Enable FPU support
CFG += -D__USE_FPU__
# Use WFI in idle task
#CFG += -D__IDLE_WFI__

# Objects specific for NUCLEOF411RE
# OBJ += drivers/uart/stm32f4xx_uart.o
OBJ += board/NUCLEOF411RE/board.o

# Tool Chain Flags and Defs
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJDUMP = arm-none-eabi-objdump
OBJDUMP_FLAGS = -S
SIZE = arm-none-eabi-size
OBJCPY = arm-none-eabi-objcopy
OBJCPY_FLAGS = -Obinary
CFLAGS = -g -Os -std=c99 -Iinclude -I. -Wall -MMD -fno-common       \
         -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
         -mlittle-endian -fsingle-precision-constant $(CFG)
LDFLAGS = -Tboard/NUCLEOF411RE/sdvos.ld -nostartfiles
BIN = $(PROGRAM).bin
DIS = $(PROGRAM).dis

# Command to upload the binary to the board
UPLOAD_COMMAND = st-flash write $(BIN) 0x8000000

