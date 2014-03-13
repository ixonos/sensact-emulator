CC=$(PREFIX)gcc
AR=$(PREFIX)ar

SA_INCLUDE_DIR=../../libsensact/src/include
SA_PLUGIN_C_FILE=../../libsensact/src/plugin.c


CFLAGS=-Wall -Wno-unused-variable -Wno-unused-but-set-variable -std=gnu99 
INCLUDE=-I./include -I$(SA_INCLUDE_DIR)


ifndef SA_INCLUDE_DIR
$(warning "You must define SA_INCLUDE_DIR to point at libsensact include dir")
endif

ifndef SA_PLUGIN_C_FILE
$(warning "You must define SA_PLUGIN_C_FILE to point at libsensact plugin.c")
endif

all: plugin.o sensact-emulator.so emulator

plugin.o: $(SA_PLUGIN_C_FILE)
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $<

emulator.o: emulator.c
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $<

emulator_sensors/emulator_sensors.a:
	make -C emulator_sensors CC=$(CC) AR=$(AR) CFLAGS="$(CFLAGS)"

emulator_tests/emulator_tests.a:
	make -C emulator_tests CC=$(CC) AR=$(AR) CFLAGS="$(CFLAGS)"

sensact-emulator.so: emulator.o emulator_sensors/emulator_sensors.a plugin.o
	$(CC) -shared -Wl,-soname,sensact-emulator.so -o sensact-emulator.so $^

emulator: emulator_tests/emulator_tests.a
	$(CC) -Wall emulator_tests/emulator_tests.a -o emulator $(LDFLAGS)

clean:
	rm -f *.o *.so
	make -C emulator_sensors clean

install:
	mkdir -p ../../sensact-server/plugins
	cp sensact-emulator.so ../../sensact-server/plugins

.PHONY: all clean install
