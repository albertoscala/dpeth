#define _GNU_SOURCE

#include "udp.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>

udp_err_t udp_create_server(const udp_config_t* config, udp_session_t* session)
{
    // Creating socket file descriptor 
    if ((session->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return UDP_SOCK_FAIL; 
    
    // bump socket buffers
    int bufsize = 26214400; // 25MB
    if (setsockopt(session->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize)) < 0)
        return UDP_SEND_BUF_FAIL;       
    if (setsockopt(session->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) < 0)
        return UDP_RECV_BUF_FAIL;       

    // Filling server information 
    session->addr.sin_family = AF_INET; // IPv4

    int err = inet_pton(AF_INET, config->addr, &session->addr.sin_addr); 
    if (err == 0) return UDP_ADDR_INVALID;
    else if (err < 0) return UDP_ADDR_FAMILY_INVALID;

    // session->addr.sin_addr.s_addr = INADDR_ANY;
    
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

    // bump socket buffers
    int bufsize = 26214400; // 25MB
    if (setsockopt(session->fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize)) < 0)
        return UDP_SEND_BUF_FAIL;
    if (setsockopt(session->fd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize)) < 0)
        return UDP_RECV_BUF_FAIL;

    // Filling server information 
    session->peeraddr.sin_family = AF_INET; // IPv4 
    
    int err = inet_pton(AF_INET, config->addr, &session->peeraddr.sin_addr);
    if (err == 0) return UDP_ADDR_INVALID;
    else if (err < 0) return UDP_ADDR_FAMILY_INVALID;
    
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

udp_err_t udp_recvmmsg(void* buf, size_t n, size_t pack_size, udp_session_t* session)
{
    int batch_size = n / pack_size;
    size_t remainder = n % pack_size;

    struct iovec iovecs[batch_size];
    struct mmsghdr msgs[batch_size];
    memset(msgs, 0, sizeof(msgs));
    
    for (int i = 0; i < batch_size; i++) 
    {
        iovecs[i].iov_base = (uint8_t*)buf + i * pack_size;
        iovecs[i].iov_len = pack_size;
        msgs[i].msg_hdr.msg_iov = &iovecs[i];
        msgs[i].msg_hdr.msg_iovlen = 1;
    }

    int total_received = 0;
    while (total_received < batch_size) 
    {
        int received = recvmmsg(session->fd, msgs + total_received,batch_size - total_received, MSG_WAITFORONE, NULL);
        if (received < 0) return UDP_RECV_FAIL;
        total_received += received;
    }

    if (remainder > 0)
    {
        if (udp_recv((uint8_t*)buf + batch_size * pack_size, remainder, session) != UDP_OK)
            return UDP_RECV_FAIL;
    }

    return UDP_OK;
}

udp_err_t udp_destroy(udp_session_t* session)
{
    if (close(session->fd) < 0)
        return UDP_CLOSE_FAIL;

    return UDP_OK;
}