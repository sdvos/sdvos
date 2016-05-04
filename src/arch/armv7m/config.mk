# Architecture specific configuration for ARMv7-M
CFG += -D__ARCH_ARMV7M__

# Objects specific for ARMv7-M
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
OBJ += arch/armv7m/cache.o
