#include "dpeth.h"

#include <stdbool.h>

udp_session_t server, client;

udp_config_t config = {
    .addr = ADDR,
    .sink_port = PORT_SINK,
    .source_port = PORT_SOURCE,
};

dpeth_err_t dpeth_create_sink(dp_connect_t* connect)
{
    if (udp_create_client(&config, &client) != UDP_OK)
    {
        printf("Couldn't create the udp server...\n");
        return DPETH_UDP_SERVER_FAIL;
    }

    //FIXME: Debug
    printf("Sink sending to port %d, listening on port %d\n", PORT_SOURCE, PORT_SINK);

    int attempts = N_ATTEMPS;
    while (attempts--)
    {
        if (udp_send(connect, sizeof(dp_connect_t), &client) != UDP_OK)
            continue;

        // RECV CONNECT_ACK
        uint8_t ack = 0x0;
        udp_err_t err = udp_recv(&ack, sizeof(uint8_t), &client);
        if (err != UDP_OK || ack != CONN_ACK)
        {
            if (err != UDP_OK) 
            { 
                printf("Retrying connection (%d attempts left)...\n", attempts);
                continue;
            }
            else 
            { 
                printf("Ack returned bad...\n"); 
                return DPETH_ACK_FAIL; 
            }
        }
        else 
        {
            return DPETH_OK;
        }
    }

    printf("Couldn't perform the handshake...\n");
    return DPETH_UDP_HANDSHAKE_FAIL;
}

dpeth_err_t dpeth_create_source(dp_connect_t* connect)
{
    if (udp_create_server(&config, &server) != UDP_OK)
    {
        printf("Couldn't create the udp client...\n");
        return DPETH_UDP_CLIENT_FAIL;
    }

    //FIXME: Debug
    printf("Source listening on port %d\n", PORT_SOURCE);

    // RECV CONNECT
    if (udp_recv(connect, sizeof(dp_connect_t), &server) != UDP_OK)
    {
        printf("Couldn't receive the requirements...\n");
        return DPETH_UDP_RECV_FAIL;
    }

    // SEND CONNECT_ACK
    uint8_t ack = CONN_ACK;
    if (udp_send(&ack, sizeof(uint8_t), &server) != UDP_OK)
    {
        printf("Couldn't send the ack...\n");
        return DPETH_ACK_SEND_FAIL;
    }

    return DPETH_OK;
}

dpeth_err_t dpeth_recv_frame(dp_connect_t* connect, uint8_t* fb)
{
    size_t fb_size = connect->width * connect->height * connect->pixel_format;
    size_t bytes_send = 0;
    while (fb_size >= PACK_SIZE)
    {
        if (udp_recv(fb + bytes_send, PACK_SIZE, &client) != UDP_OK)
        {
            //TODO: Handle
        }
        bytes_send += PACK_SIZE;
        fb_size -= PACK_SIZE;
    }
    if (fb_size > 0)
    {
        if (udp_recv(fb + bytes_send, fb_size, &client) != UDP_OK)
        {
            //TODO: Handle
        }
    }

    //TODO: Send the ACK (complete frame)
    uint8_t ack = FRAME_ACK;
    if (udp_send(&ack, sizeof(uint8_t), &client) != UDP_OK)
    {
        printf("Couldn't send the frame ack...\n");
        return DPETH_ACK_FRAME_FAIL;
    }

    return DPETH_OK;
}

dpeth_err_t dpeth_send_frame(dp_connect_t* connect, const uint8_t* fb)
{
    size_t fb_size = connect->width * connect->height * connect->pixel_format;
    size_t bytes_recv = 0;
    while (fb_size >= PACK_SIZE)
    {
        if (udp_send(fb + bytes_recv, PACK_SIZE, &server) != UDP_OK)
        {
            //TODO: Handle
        }
        bytes_recv += PACK_SIZE;
        fb_size -= PACK_SIZE;
    }
    if (fb_size > 0)
    {
        if (udp_send(fb + bytes_recv, fb_size, &server) != UDP_OK)
        {
            //TODO: Handle
        }
    }

    // RECV CONNECT_ACK
    uint8_t ack = 0x0;
    udp_err_t err = udp_recv(&ack, sizeof(uint8_t), &server);

    printf("ACK ARRIVATO FORSE!!!\n");

    if (err != UDP_OK || ack != FRAME_ACK)
    {
        if (err != UDP_OK) 
        { 
            printf("Couldn't receive the ACK...");
            return DPETH_UDP_RECV_FAIL;
        }
        else
        { 
            printf("Frame ACK returned bad...\n"); 
            return DPETH_ACK_FRAME_FAIL; 
        }
    }

    return DPETH_OK;
}

dpeth_err_t dpeth_recv_loop()
{
    return DPETH_OK;
}

dpeth_err_t dpeth_send_loop()
{
    return DPETH_OK;
}

dpeth_err_t dpeth_close_sink()
{
    return DPETH_OK;
}

dpeth_err_t dpeth_close_source()
{
    return DPETH_OK;
}