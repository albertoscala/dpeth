#include "../include/dpeth.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_FRAMES 150  // 30 seconds at 5fps

// --- async frame save ---

typedef struct {
    uint8_t* buf;
    size_t   size;
    int      frame_id;
} save_job_t;

void* save_thread(void* arg)
{
    save_job_t* job = arg;

    char filename[64];
    snprintf(filename, sizeof(filename), "frame_%04d.raw", job->frame_id);

    FILE* f = fopen(filename, "wb");
    if (f)
    {
        fwrite(job->buf, 1, job->size, f);
        fclose(f);
        printf("Saved %s\n", filename);
    }
    else
    {
        printf("Failed to open %s for writing\n", filename);
    }

    free(job->buf);
    free(job);
    return NULL;
}

void save_frame_async(const uint8_t* fb, size_t size, int frame_id)
{
    save_job_t* job = malloc(sizeof(save_job_t));
    if (!job) { printf("OOM: save_job_t\n"); return; }

    job->buf = malloc(size);
    if (!job->buf) { printf("OOM: frame buf\n"); free(job); return; }

    job->size     = size;
    job->frame_id = frame_id;
    memcpy(job->buf, fb, size);   // copy so network can reuse fb immediately

    pthread_t t;
    pthread_create(&t, NULL, save_thread, job);
    pthread_detach(t);            // fire and forget
}

// --- main ---

int main()
{
    dp_connect_t connect;
    memset(&connect, 0, sizeof(dp_connect_t));
    connect.width        = WIDTH;
    connect.height       = HEIGHT;
    connect.fps          = FPS;
    connect.pixel_format = RGB888;
    connect.color_depth  = SDR;

    dpeth_create_sink(&connect);

    size_t   fb_size = connect.width * connect.height * connect.pixel_format;
    uint8_t* fb      = malloc(fb_size);
    if (!fb) { printf("OOM: framebuffer\n"); return 1; }

    for (int i = 0; ; ++i)
    {
        if (dpeth_recvmmsg_frame(&connect, fb) != DPETH_OK)
        {
            printf("Frame %d recv failed, skipping\n", i);
            continue;
        }

        printf("Frame %d received\n", i);

        save_frame_async(fb, fb_size, i);
    }

    free(fb);
    return 0;
}