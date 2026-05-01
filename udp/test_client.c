#include "udp.h"

#include <stdio.h>
#include <string.h>

#define BUF_SIZE 50

int main()
{
    udp_session_t client;
    memset(&client, 0, sizeof(client));

    (void)udp_create_client("127.0.0.1", 6767, &client);

    char buf[BUF_SIZE];
    strcpy(buf, "Hi from Client!");
    (void)udp_send(buf, BUF_SIZE, &client);

    return 0;
}