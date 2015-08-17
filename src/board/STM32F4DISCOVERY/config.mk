# Board Specific Configuration for STM32F4DISCOVERY
CFG += -D__ARCH_ARMV7M__
# 64K CCM, 112K+16K contiguous on AHB, 4K battery
CFG += -DARCH_SRAM_END=0x20020000
CFG += -DKERN_STK_SIZE=0x100
CFG += -DIDLE_STK_SIZE=0x100
# Enable FPU support
CFG += -D__USE_FPU__
# Use WFI in idle task
#CFG += -D__IDLE_WFI__

# Objects specific for STM32F4DISCOVERY
OBJ += arch/armv7m/task.o
OBJ += arch/armv7m/panic.o
OBJ += arch/armv7m/interrupt.o
OBJ += arch/armv7m/syscall.o
OBJ += arch/armv7m/timer.o
OBJ += arch/armv7m/idle.o
OBJ += arch/armv7m/utils.o
OBJ += arch/armv7m/start.o
OBJ += arch/armv7m/svc.o
OBJ += arch/armv7m/mcu.o
# OBJ += drivers/uart/stm32f4xx_uart.o
OBJ += board/STM32F4DISCOVERY/board.o

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
LDFLAGS = -Tboard/STM32F4DISCOVERY/sdvos.ld -nostartfiles
BIN = $(PROGRAM).bin
DIS = $(PROGRAM).dis

# Command to upload the binary to the board
UPLOAD_COMMAND = st-flash write $(BIN) 0x8000000

