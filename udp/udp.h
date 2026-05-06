#ifndef UDP_H
#define UDP_H

#include <stdlib.h>
#include <netinet/in.h>

typedef enum 
{
    UDP_OK = 0,
    UDP_SOCK_FAIL,
    UDP_ADDR_INVALID,
    UDP_ADDR_FAMILY_INVALID,
    UDP_SOCKOPT_FAIL,
    UDP_BIND_FAIL,
    UDP_SEND_FAIL,
    UDP_RECV_FAIL,
    UDP_CLOSE_FAIL
} udp_err_t;

typedef struct 
{
    const char* addr;
    int sink_port;
    int source_port;
} udp_config_t;

typedef struct 
{
    int fd;
    struct sockaddr_in addr;
    struct sockaddr_in peeraddr;
} udp_session_t;

udp_err_t udp_create_server(const udp_config_t* config, udp_session_t* session);
udp_err_t udp_create_client(const udp_config_t* config, udp_session_t* session);

udp_err_t udp_send(const void* buf, size_t n, udp_session_t* session);
udp_err_t udp_recv(void* buf, size_t n, udp_session_t* session);

udp_err_t udp_batch_ctx_init(size_t fb_size);
void udp_batch_ctx_free();
udp_err_t udp_recv_batch(void* fb, size_t fb_size, size_t chunk_size, udp_session_t* session);

udp_err_t udp_destroy(udp_session_t* session);

#endif /* UDP_H */