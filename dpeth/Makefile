CC = gcc
CFLAGS = -Wall -g
UDP = ../udp/udp.c
TCP = ../tcp/tcp.c

display:
	$(CC) $(CFLAGS) -o display display.c dpeth.c $(UDP) $(TCP)

source:
	$(CC) $(CFLAGS) -o source source.c dpeth.c $(UDP) $(TCP) -lm

all: display source

clean:
	rm -f display source frame_test