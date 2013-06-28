CC=gcc
CFLAGS=-g -c -Wall -DFIT_USE_STDINT_H

LD=gcc
LDFLAGS=-L. -lfit

EXE=fhead

all: $(EXE)

$(EXE): main.o libfit.so util.o
	$(LD) $(LDFLAGS) main.o util.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

libfit.so:
	$(CC) -shared -fPIC -O3 -Wall -Wpadded -fpack-struct \
		-o $@ \
		sdk/fit_crc.c \
		sdk/fit_convert.c \
		sdk/fit_product.c \
		sdk/fit_ram.c \
		sdk/fit.c

.PHONY: clean

clean:
	rm -f *.o *.so $(EXE)
