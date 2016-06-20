# Include AVR generic configuration
-include arch/avr/config.mk

# Architecture specific configuration for AVR6
CFG += -D__ARCH_AVR6__
# Page size for AVR6. If not set, default to 128 bytes.
#CFG += -DPGSIZE=0x80
#CFG += -DWITH_FLASH_UTILITY

# Objects specific for AVR6
OBJ += arch/avr6/task.o
