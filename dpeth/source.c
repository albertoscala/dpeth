#define _POSIX_C_SOURCE 199309L
#include "dpeth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define NS_PER_SEC  1000000000L

int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));

    // RECV CONNECT AND SEND THE ACK
    dpeth_create_source(&connect);

    const float frame_ns = NS_PER_SEC / connect.fps;

    struct timespec frame_start, frame_end;

    uint8_t* fb = malloc(connect.width * connect.height * connect.pixel_format);

    // FEED
    for (int i = 0; ; ++i)
    {
        clock_gettime(CLOCK_MONOTONIC, &frame_start);

        float t = i * 0.05f;

        for (int y = 0; y < connect.height; ++y)
        {
            for (int x = 0; x < connect.width; ++x)
            {
                // Plasma formula: sum of sine waves
                float v = sinf(x * 0.02f + t)
                        + sinf(y * 0.03f + t * 1.3f)
                        + sinf((x + y) * 0.025f + t * 0.7f)
                        + sinf(sqrtf((x - 400.0f)*(x - 400.0f) + (y - 300.0f)*(y - 300.0f)) * 0.04f - t);

                // v is in [-4, 4], normalize to [0, 255]
                uint8_t r = (uint8_t)(128 + 64 * sinf(v));
                uint8_t g = (uint8_t)(128 + 64 * sinf(v + 2.094f)); // 120 deg offset
                uint8_t b = (uint8_t)(128 + 64 * sinf(v + 4.189f)); // 240 deg offset

                int idx = (y * connect.width + x) * 3;
                fb[idx + 0] = r;
                fb[idx + 1] = g;
                fb[idx + 2] = b;
            }
        }

        dpeth_send_frame(&connect, fb);

        printf("Frame %d sent\n", i);

        clock_gettime(CLOCK_MONOTONIC, &frame_end);

        long elapsed = (frame_end.tv_sec - frame_start.tv_sec) * NS_PER_SEC
                 + (frame_end.tv_nsec - frame_start.tv_nsec);

        long remaining = frame_ns - elapsed;
        if (remaining > 0)
        {
            struct timespec sleep_time = {
                .tv_sec  = remaining / NS_PER_SEC,
                .tv_nsec = remaining % NS_PER_SEC
            };
            nanosleep(&sleep_time, NULL);
        }

    }

    // CLOSE

    return 0;
}