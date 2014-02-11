
objects = sensact-emulator-test.o

all: emulator_tests.a

$(objects): %.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

emulator_tests.a: $(objects)
	$(AR) rcs $@ $<

clean:
	rm -f *.o *.a

.PHONY: all clean
