#include "dpeth.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));

    // RECV CONNECT AND SEND THE ACK
    dpeth_create_source(&connect);

    uint8_t* fb = malloc(WIDTH * HEIGHT * (1 + RGB888));

    // FEED
    for (int i = 0; ; ++i)
    {
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                if (i%2 == 0)
                {
                    if ((x%(WIDTH/8))%2==0)
                    {
                        fb[(y * WIDTH + x) * 4 + 0] = 255;
                        fb[(y * WIDTH + x) * 4 + 1] = 255;
                        fb[(y * WIDTH + x) * 4 + 2] = 255;
                        fb[(y * WIDTH + x) * 4 + 3] = 255;
                    }
                    else
                    {
                        fb[(y * WIDTH + x) * 4 + 0] = 255;
                        fb[(y * WIDTH + x) * 4 + 1] = 0;
                        fb[(y * WIDTH + x) * 4 + 2] = 0;
                        fb[(y * WIDTH + x) * 4 + 3] = 0;
                    }  
                }
                else 
                {
                    if ((x%(WIDTH/8))%2==0)
                    {
                        fb[(y * WIDTH + x) * 4 + 0] = 255;
                        fb[(y * WIDTH + x) * 4 + 1] = 0;
                        fb[(y * WIDTH + x) * 4 + 2] = 0;
                        fb[(y * WIDTH + x) * 4 + 3] = 0;
                    }
                    else
                    {
                        fb[(y * WIDTH + x) * 4 + 0] = 255;
                        fb[(y * WIDTH + x) * 4 + 1] = 255;
                        fb[(y * WIDTH + x) * 4 + 2] = 255;
                        fb[(y * WIDTH + x) * 4 + 3] = 255;
                    }
                }
            }   
        }

        dpeth_send_frame(&connect, fb);
    }

    // CLOSE

    return 0;
}