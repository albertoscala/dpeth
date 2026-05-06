#ifndef DPETH_H
#define DPETH_H

#include "../udp/udp.h"

#include <stdint.h>
#include <stdio.h>

#define listen dpeth_recv_loop
#define feed dpeth_send_loop

#define ADDR "192.168.1.3"
#define PORT_SINK 6767
#define PORT_SOURCE 6767

#define PACK_SIZE 65507

#define N_ATTEMPS 15

#define WIDTH 1920
#define HEIGHT 1080
#define FPS 20

#define CONN_ACK 0xFF
#define FRAME_ACK 0xAA

typedef enum
{
    DPETH_OK = 0,
    DPETH_UDP_SERVER_FAIL,
    DPETH_UDP_CLIENT_FAIL,
    DPETH_UDP_HANDSHAKE_FAIL,
    DPETH_ACK_SEND_FAIL,
    DPETH_ACK_FRAME_FAIL,
    DPETH_UDP_RECV_FAIL,
    DPETH_UDP_SEND_FAIL,
    DPETH_ACK_FAIL,
} dpeth_err_t;

typedef enum
{
    RGB888 = 3,
    ARGB8888 = 4,
    YUV444 = 6,
    YUV422 = 3,
} dpeth_format_t;

typedef enum
{
    SDR = 8,    // 8bit color depth
    HDR = 10,   // 10bit color depth 
    HDRP = 12,  // 12bit color depth
    HBD = 16    // 16bit color depth (medical/scientific image)
} dpeth_depth_t;

// CONNECT payload — the only "negotiation" you need
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t fps;
    dpeth_format_t pixel_format;  // 0=RGB24, 1=YUV422, 2=YUV444
    dpeth_depth_t color_depth;   // bits per channel
} __attribute__((packed)) dp_connect_t;

dpeth_err_t dpeth_create_sink(dp_connect_t* connect);
dpeth_err_t dpeth_create_source(dp_connect_t* connect);

dpeth_err_t dpeth_recv_frame(dp_connect_t* connect, uint8_t* fb);
dpeth_err_t dpeth_send_frame(dp_connect_t* connect, const uint8_t* fb);

dpeth_err_t dpeth_recv_loop();
dpeth_err_t dpeth_send_loop();

dpeth_err_t dpeth_close_sink();
dpeth_err_t dpeth_close_source();

#endif /* DPETH_H */
