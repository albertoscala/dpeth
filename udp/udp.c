#include "udp.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>

// MACOS compatibility
#ifdef __APPLE__
#include <unistd.h>
#define MSG_CONFIRM 0
#endif

udp_err_t udp_create_server(const udp_config_t* config, udp_session_t* session)
{
    // Creating socket file descriptor 
    if ((session->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return UDP_SOCK_FAIL; 
      
    // Filling server information 
    session->addr.sin_family = AF_INET; // IPv4

    // int err = inet_pton(AF_INET, config->addr, &session->addr.sin_addr); 
    // if (err == 0)
    //     return UDP_ADDR_INVALID;
    // else if (err < 0)
    //     return UDP_ADDR_FAMILY_INVALID;

    session->addr.sin_addr.s_addr = INADDR_ANY;
    
    session->addr.sin_port = htons(config->source_port);
      
    // Bind the socket with the server address 
    if (bind(session->fd, (const struct sockaddr *)&session->addr, sizeof(session->addr)) < 0) 
        return UDP_BIND_FAIL;

    return UDP_OK;
}

udp_err_t udp_create_client(const udp_config_t* config, udp_session_t* session)
{
    // Create UDP socket
    if ((session->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        return UDP_SOCK_FAIL;

    // Filling server information 
    session->peeraddr.sin_family = AF_INET; // IPv4 
    
    int err = inet_pton(AF_INET, config->addr, &session->peeraddr.sin_addr);
        if (err == 0)
        return UDP_ADDR_INVALID;
    else if (err < 0)
        return UDP_ADDR_FAMILY_INVALID;
    
    session->peeraddr.sin_port = htons(config->source_port);

    // Filling our information 
    session->addr.sin_family = AF_INET; // IPv4
    session->addr.sin_addr.s_addr = INADDR_ANY; // Listen to all interfaces 
    session->addr.sin_port = htons(config->sink_port);

    // Bind to port to allow stable connection
    if (bind(session->fd, (const struct sockaddr*)&session->addr, sizeof(session->addr)) < 0)
        return UDP_BIND_FAIL;

    struct timeval timeout;
    memset(&timeout, 0, sizeof(struct timeval));
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(session->fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        return UDP_SOCKOPT_FAIL;

    return UDP_OK;
}

udp_err_t udp_send(const void* buf, size_t n, udp_session_t* session)
{
    socklen_t len = sizeof(struct sockaddr_in);
    if (sendto(session->fd, buf, n, MSG_CONFIRM, (const struct sockaddr *)&session->peeraddr, len) < 0)
        return UDP_SEND_FAIL;

    return UDP_OK;
}

udp_err_t udp_recv(void* buf, size_t n, udp_session_t* session)
{
    socklen_t len = sizeof(struct sockaddr_in);
    if (recvfrom(session->fd, buf, n, MSG_WAITALL, (struct sockaddr *)&session->peeraddr, &len) < 0)
        return UDP_RECV_FAIL;

    return UDP_OK;
}

udp_err_t udp_destroy(udp_session_t* session)
{
    if (close(session->fd) < 0)
        return UDP_CLOSE_FAIL;

    return UDP_OK;
}