# Include AVR generic configuration
-include arch/avr/config.mk

# Architecture specific configuration for AVR5
CFG += -D__ARCH_AVR5__
# Page size for AVR5. If not set, default to 128 bytes.
#CFG += -DPGSIZE=0x80
#CFG += -DWITH_FLASH_UTILITY

# Objects specific for AVR5
OBJ += arch/avr5/task.o
#OBJ += arch/avr5/pgm_mem.o
