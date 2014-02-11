CC=$(PREFIX)gcc
AR=$(PREFIX)ar
CFLAGS=-Wall -Wno-unused-variable -Wno-unused-but-set-variable -std=gnu99 
INCLUDE=-I./include -I$(SA_INCLUDE_DIR)

ifndef SA_INCLUDE_DIR
$(error "You must define SA_INCLUDE_DIR to point at libsensact include dir")
endif

ifndef SA_PLUGIN_C_FILE
$(error "You must define SA_PLUGIN_C_FILE to point at libsensact plugin.c")
endif

all: plugin.o sensact-emulator.so

plugin.o: $(SA_PLUGIN_C_FILE)
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $(SA_PLUGIN_C_FILE)

emulator_sensors/emulator_sensors.a:
	make -C emulator_sensors emulator_sensors.a

sensact-emulator.so: emulator.c plugin.o emulator_sensors/emulator_sensors.a
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c emulator.c 
	$(CC) -shared -Wl,-soname,sensact-emulator.so -o sensact-emulator.so emulator.o plugin.o emulator_sensors/emulator_sensors.a

emulator: emulator_tests/sensact-emulator-test.o
	$(CC) -Wall emulator_tests/sensact-emulator-test.o -o emulator $(LDFLAGS)

clean:
	rm -f *.o *.so
	make -C emulator_sensors clean

.PHONY: all clean
