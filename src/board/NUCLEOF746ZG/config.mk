# Board Specific Configuration for NUCLEOF746ZG
-include arch/armv7m/config.mk

CFG += -DARCH_SRAM_END=0x20050000
CFG += -DKERN_STK_SIZE=0x100
CFG += -DIDLE_STK_SIZE=0x100
# Enable FPU support
CFG += -D__USE_FPU__
# Enable Cache support
CFG += -D__USE_CACHE__
# Use WFI in idle task
#CFG += -D__IDLE_WFI__

# Objects specific for NUCLEOF746ZG
OBJ += board/NUCLEOF746ZG/board.o

# Tool Chain Flags and Defs
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJDUMP = arm-none-eabi-objdump
OBJDUMP_FLAGS = -S
SIZE = arm-none-eabi-size
OBJCPY = arm-none-eabi-objcopy
OBJCPY_FLAGS = -Obinary
CFLAGS = -g -Os -std=c99 -Iinclude -I. -Wall -MMD -fno-common       \
         -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
         -mlittle-endian -fsingle-precision-constant                \
         -mno-unaligned-access $(CFG)
LDFLAGS = -Tboard/NUCLEOF746ZG/sdvos.ld -nostartfiles
BIN = $(PROGRAM).bin
DIS = $(PROGRAM).dis

# Command to upload the binary to the board
UPLOAD_COMMAND = st-flash write $(BIN) 0x8000000

