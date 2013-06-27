CC=gcc
CFLAGS=-g -c -Wall

LD=gcc
LDFLAGS=

EXE=fhead

all: $(EXE)

$(EXE): main.o fit.o
	$(LD) $(LDFLAGS) main.o fit.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f *.o $(EXE)

