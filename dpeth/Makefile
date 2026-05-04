CC = gcc
CFLAGS = -Wall -g $(shell sdl2-config --cflags)
LIBS_SDL = $(shell sdl2-config --libs)
UDP = ../udp/udp.c

sink:
	$(CC) $(CFLAGS) -o display display.c dpeth.c $(UDP) $(LIBS_SDL)
source:
	$(CC) $(CFLAGS) -o source source.c dpeth.c $(UDP)
frame_test:
	$(CC) $(CFLAGS) -o frame_test frame_test.c -lX11
all: sink source
clean:
	rm -f sink source frame_test