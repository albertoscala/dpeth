#include "udp.h"

#include <stdio.h>
#include <string.h>

#define BUF_SIZE 50

int main()
{
    udp_session_t server;
    memset(&server, 0, sizeof(server));

    (void)udp_create_server("127.0.0.1", 6767, &server);

    char buf[BUF_SIZE];
    (void)udp_recv(buf, BUF_SIZE, &server);

    printf("Message received: %s\n", buf);

    return 0;
}