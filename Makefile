CC = gcc
CFLAGS = -Wall -g -O3
SRC = src
UDP = src/udp.c
TCP = src/tcp.c
DPETH = src/dpeth.c
BIN = bin
LIBS = -lm -lpthread

display:
	$(CC) $(CFLAGS) -o $(BIN)/display $(SRC)/display.c $(DPETH) $(UDP) $(TCP) $(LIBS)

source:
	$(CC) $(CFLAGS) -o $(BIN)/source $(SRC)/source.c $(DPETH) $(UDP) $(TCP) $(LIBS)

all: display source

clean:
	rm -f $(BIN)/*
