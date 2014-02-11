
objects = sensact_emulator_engine.o sensact_emulator_senshub.o sensact_emulator_ble.o

all: emulator_sensors.a

$(objects): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

emulator_sensors.a: $(objects)
	$(AR) rcs $@ $<

clean:
	rm -f *.o *.a

.PHONY: all clean
