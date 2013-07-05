CC=gcc
CFLAGS=-g -c -Wall -DFIT_USE_STDINT_H

LD=gcc
LDFLAGS=-lfit

EXE=fhead

all: $(EXE)

$(EXE): main.o util.o output.o
	$(LD) $(LDFLAGS) main.o util.o output.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o *.so $(EXE)
