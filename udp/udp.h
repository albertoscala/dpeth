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
    UDP_BIND_FAIL,
    UDP_SEND_FAIL,
    UDP_RECV_FAIL,
    UDP_CLOSE_FAIL
} udp_err_t;

typedef struct 
{
    int fd;
    struct sockaddr_in servaddr;
} udp_t;

udp_err_t udp_create_server(const char* addr, int port, udp_t* server);
udp_err_t udp_create_client(const char* addr, int port, udp_t* client);

udp_err_t udp_send(const void* buf, size_t n, udp_t* session);
udp_err_t udp_recv(void* buf, size_t n, udp_t* session);

udp_err_t udp_destroy(udp_t* session);

#endif /* UDP_H */