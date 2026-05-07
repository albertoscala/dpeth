#define _POSIX_C_SOURCE 199309L
#include "../include/dpeth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define NS_PER_SEC          1000000000L
#define FPS_CHECK_INTERVAL  20
#define N_THREADS           4
#define SIN_TABLE_SIZE      4096

#define M_PI    3.14159265358979323846264338327950288   /**< pi */


// ---------------------------------------------------------------------------
// Fast sin lookup table [-1, 1] over [0, 2*PI]
// ---------------------------------------------------------------------------
static float sin_table[SIN_TABLE_SIZE];

static void build_sin_table(void)
{
    for (int i = 0; i < SIN_TABLE_SIZE; ++i)
        sin_table[i] = sinf(2.0f * M_PI * i / SIN_TABLE_SIZE);
}

static inline float fsin(float x)
{
    // wrap x into [0, 1) then index
    x = x * (SIN_TABLE_SIZE / (2.0f * M_PI));
    int idx = (int)x & (SIN_TABLE_SIZE - 1);
    return sin_table[(idx + SIN_TABLE_SIZE) & (SIN_TABLE_SIZE - 1)];
}

// ---------------------------------------------------------------------------
// Per-thread render context
// ---------------------------------------------------------------------------
typedef struct
{
    int        y_start;
    int        y_end;
    float      t;
    uint8_t*   fb;
    int        width;
    int        height;
    float*     dist_table;   // precomputed sqrtf * 0.04f per pixel
} tile_args_t;

static void* render_tile(void* arg)
{
    tile_args_t* a = arg;
    const float t        = a->t;
    const int   width    = a->width;
    const float C1       = 2.094f;   // 2*PI/3
    const float C2       = 4.189f;   // 4*PI/3

    for (int y = a->y_start; y < a->y_end; ++y)
    {
        // hoist y-only terms out of the inner loop
        float sy   = fsin(y * 0.03f + t * 1.3f);
        float sxy0 = y * 0.025f + t * 0.7f;   // (x+y)*0.025 base

        const float* dist_row = a->dist_table + y * width;
        uint8_t*     row      = a->fb + y * width * 3;

        for (int x = 0; x < width; ++x)
        {
            float v = fsin(x * 0.02f + t)
                    + sy
                    + fsin(x * 0.025f + sxy0)
                    + fsin(dist_row[x] - t);

            row[x*3 + 0] = (uint8_t)(128 + 64 * fsin(v));
            row[x*3 + 1] = (uint8_t)(128 + 64 * fsin(v + C1));
            row[x*3 + 2] = (uint8_t)(128 + 64 * fsin(v + C2));
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));
    dpeth_create_source(&connect);

    const int   W        = connect.width;
    const int   H        = connect.height;
    const float frame_ns = (float)NS_PER_SEC / connect.fps;

    build_sin_table();

    // precompute radial distance table (sqrtf only done once at startup)
    float* dist_table = malloc(W * H * sizeof(float));
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
        {
            float dx = x - 400.0f, dy = y - 300.0f;
            dist_table[y * W + x] = sqrtf(dx*dx + dy*dy) * 0.04f;
        }

    uint8_t* fb = malloc(W * H * connect.pixel_format);

    pthread_t   threads[N_THREADS];
    tile_args_t args[N_THREADS];
    int slice = H / N_THREADS;

    struct timespec frame_start, frame_end, fps_epoch;
    clock_gettime(CLOCK_MONOTONIC, &fps_epoch);

    for (int i = 0; ; ++i)
    {
        clock_gettime(CLOCK_MONOTONIC, &frame_start);
        float t = i * 0.05f;

        // spawn threads
        for (int k = 0; k < N_THREADS; ++k)
        {
            args[k] = (tile_args_t){
                .y_start    = k * slice,
                .y_end      = (k == N_THREADS - 1) ? H : (k + 1) * slice,
                .t          = t,
                .fb         = fb,
                .width      = W,
                .height     = H,
                .dist_table = dist_table,
            };
            pthread_create(&threads[k], NULL, render_tile, &args[k]);
        }
        for (int k = 0; k < N_THREADS; ++k)
            pthread_join(threads[k], NULL);

        dpeth_send_frame(&connect, fb);

        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        long elapsed = (frame_end.tv_sec  - frame_start.tv_sec)  * NS_PER_SEC
                     + (frame_end.tv_nsec - frame_start.tv_nsec);

        // FPS report
        if (i > 0 && i % FPS_CHECK_INTERVAL == 0)
        {
            long window = (frame_end.tv_sec  - fps_epoch.tv_sec)  * NS_PER_SEC
                        + (frame_end.tv_nsec - fps_epoch.tv_nsec);
            float actual_fps = FPS_CHECK_INTERVAL / (window / (float)NS_PER_SEC);
            printf("Frame %4d | FPS: %5.2f (target: %d) | frame time: %5.2fms | drift: %+.2fms%s\n",
                   i, actual_fps, connect.fps,
                   elapsed / 1e6f,
                   (elapsed - frame_ns) / 1e6f,
                   fabsf(actual_fps - connect.fps) > 1.0f ? "  *** FPS OFF ***" : "");
            fps_epoch = frame_end;
        }

        // absolute deadline sleep — no drift accumulation
        long remaining = (long)frame_ns - elapsed;
        if (remaining > 0)
        {
            struct timespec deadline = frame_start;
            deadline.tv_nsec += (long)frame_ns;
            if (deadline.tv_nsec >= NS_PER_SEC)
            {
                deadline.tv_sec++;
                deadline.tv_nsec -= NS_PER_SEC;
            }
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
        }
    }

    free(dist_table);
    free(fb);
    return 0;
}