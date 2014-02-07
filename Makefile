CC=$(PREFIX)gcc
CFLAGS=-Wall -Wno-unused-variable -Wno-unused-but-set-variable

SA_INCLUDE=-I../include
SA_PLUGIN_C_FILE=../plugin.c

all: plugin.o sensact-emulator.so

plugin.o: $(SA_PLUGIN_C_FILE)
	$(CC) $(CFLAGS) $(SA_INCLUDE) -fPIC -c $(SA_PLUGIN_C_FILE)

sensact-emulator.so: emulator.c plugin.o
	$(CC) $(CFLAGS) $(SA_INCLUDE) -fPIC -c emulator.c
	$(CC) -shared -Wl,-soname,sensact-emulator.so -o sensact-emulator.so emulator.o plugin.o

clean:
	rm -f *.o *.so

.PHONY: all clean
