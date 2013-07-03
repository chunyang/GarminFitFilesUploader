CC=gcc
CFLAGS=-g -c -Wall -DFIT_USE_STDINT_H

LD=gcc
LDFLAGS=-L. -lfit

SDKFILES=sdk/fit_crc.c sdk/fit_convert.c sdk/fit_product.c sdk/fit_ram.c sdk/fit.c

EXE=fhead

all: $(EXE)

$(EXE): main.o libfit.so util.o output.o
	$(LD) $(LDFLAGS) main.o util.o output.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

libfit.so: $(SDKFILES)
	$(CC) -DFIT_USE_STDINT_H -shared -fPIC -O3 -Wall -Wpadded -fpack-struct \
		$(SDKFILES) -o $@

decode: sdk/examples/decode/decode.c libfit.so
	$(LD) $(LDFLAGS) sdk/examples/decode/decode.c -o $@

.PHONY: clean

clean:
	rm -f *.o *.so $(EXE)
