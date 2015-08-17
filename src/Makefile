PROGRAM = sdvos
# Include generic SDVOS configuration file
-include config.mk
# Include board specific configuration file
-include board/$(BOARD)/config.mk
# Include application specific configuration file
-include apps/config.mk

DEPS = $(patsubst %.o,%.d,$(OBJ))

CFLAGS += -Iboard/$(BOARD)

all: $(PROGRAM) $(BIN) $(DIS) tags

$(DIS): $(BIN)
	$(OBJDUMP) $(OBJDUMP_FLAGS) $(PROGRAM) > $(DIS)

$(BIN): $(PROGRAM)
	$(OBJCPY) $(OBJCPY_FLAGS) $(PROGRAM) $(BIN)

$(OBJ): config.mk board/$(BOARD)/config.mk
$(PROGRAM): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

size: $(PROGRAM)
	$(SIZE) -t $(PROGRAM)

tags:
	ctags -R .

clean:
	rm -rf $(PROGRAM) $(OBJ) $(BIN) $(DIS) $(DEPS) tags
clean-config:
	rm -rf config.mk config apps/config.mk

burn: $(BIN)
	$(UPLOAD_COMMAND)

config.mk:
	$(warning config.mk does not exist!)
	$(error Please run system generator before running make!)
board/$(BOARD)/config.mk:
	$(warning board/$(BOARD)/config.mk does not exist!)
	$(error Please run system generator before running make!)
apps/config.mk:
	$(warning apps/config.mk does not exist!)
	$(error Please run system generator before running make!)

-include $(DEPS)

