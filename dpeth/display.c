#include "dpeth.h"

#include <stdio.h>
#include <string.h>

int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));

    connect.width = WIDTH;
    connect.height = HEIGHT;
    connect.fps = FPS;
    connect.pixel_format = RGB888;
    connect.color_depth = SDR;

    // SEND CONNECT AND RECV THE ACK
    dpeth_create_sink(&connect);

    // SET FRAMEBUFFER (SET THE SOURCE FOR THE CATCH)

    // LISTEN

    // CLOSE

    return 0;
}