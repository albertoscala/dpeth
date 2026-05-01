#include "dpeth.h"

#include <stdio.h>

int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));

    // RECV CONNECT AND SEND THE ACK
    dpeth_create_source(&connect);

    // FEED

    // CLOSE

    return 0;
}