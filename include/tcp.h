#ifndef TCP_H
#define TCP_H

#include <stdlib.h>
#include <netinet/in.h>

typedef enum 
{
    TCP_OK = 0,
    TCP_SOCK_FAIL,
    TCP_ADDR_INVALID,
    TCP_ADDR_FAMILY_INVALID,
    TCP_SOCKOPT_FAIL,
    TCP_BIND_FAIL,
    TCP_LISTEN_FAIL,
    TCP_ACCEPT_FAIL,
    TCP_CONNECT_FAIL,
    TCP_SEND_FAIL,
    TCP_RECV_FAIL,
    TCP_CLOSE_FAIL
} tcp_err_t;

typedef struct 
{
    const char* addr;
    int sink_port;
    int source_port;
} tcp_config_t;

typedef struct 
{
    int fd;
    int listen_fd;
    struct sockaddr_in addr;
    struct sockaddr_in peeraddr;
} tcp_session_t;

tcp_err_t tcp_create_server(const tcp_config_t* config, tcp_session_t* session);
tcp_err_t tcp_create_client(const tcp_config_t* config, tcp_session_t* session);

tcp_err_t tcp_send(const void* buf, size_t n, tcp_session_t* session);
tcp_err_t tcp_recv(void* buf, size_t n, tcp_session_t* session);

tcp_err_t tcp_destroy(tcp_session_t* session);

#endif /* TCP_H */