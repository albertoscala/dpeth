#ifndef DPETH_H
#define DPETH_H

#include <stdint.h>

#define listen dpeth_recv_loop
#define feed dpeth_send_loop

typedef enum
{
    DPETH_OK = 0,
} dpeth_err_t;

typedef enum
{
    RGB888 = 0,
    YUV444,
    YUV422,
} dpeth_format_t;

typedef enum
{
    SDR = 0,    // 8bit color depth
    HDR,        // 10bit color depth 
    HDRP,       // 12bit color depth
    HBD         // 16bit color depth (medical/scientific image)
} dpeth_depth_t;

// CONNECT payload — the only "negotiation" you need
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t fps;
    dpeth_format_t pixel_format;  // 0=RGB24, 1=YUV422, 2=YUV444
    dpeth_depth_t color_depth;   // bits per channel
} __attribute__((packed)) dp_connect_t;

dpeth_err_t dpeth_create_sink();
dpeth_err_t dpeth_create_source();

dpeth_err_t dpeth_recv_frame();
dpeth_err_t dpeth_send_frame();

dpeth_err_t dpeth_recv_loop();
dpeth_err_t dpeth_send_loop();

dpeth_err_t dpeth_close_sink();
dpeth_err_t dpeth_close_source();

#endif /* DPETH_H */