CC = gcc
CFLAGS = -Wall -g

all: server client

server:
	$(CC) $(CFLAGS) -o server test_server.c udp.c

client: 
	$(CC) $(CFLAGS) -o client test_client.c udp.c

clean:
	rm -f server client