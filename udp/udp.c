#include "udp.h"

#include <sys/socket.h>
#include <arpa/inet.h>

udp_err_t udp_create_server(const char* addr, int port, udp_t* server)
{
    // Creating socket file descriptor 
    if ((server->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        return UDP_SOCK_FAIL; 
      
    // Filling server information 
    server->servaddr.sin_family = AF_INET; // IPv4

    int err = inet_pton(AF_INET, addr, &server->servaddr.sin_addr); 
    if (err == 0)
        return UDP_ADDR_INVALID;
    else if (err < 0)
        return UDP_ADDR_FAMILY_INVALID;
    
        server->servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address 
    if (bind(server->fd, (const struct sockaddr *)&server->servaddr, sizeof(server->servaddr)) < 0) 
        return UDP_BIND_FAIL;

    return UDP_OK;
}

udp_err_t udp_create_client(const char* addr, int port, udp_t* client)
{
    // Create UDP socket
    if ((client->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        return UDP_SOCK_FAIL;

    // Filling server information 
    client->servaddr.sin_family = AF_INET; // IPv4 
    
    int err = inet_pton(AF_INET, addr, &client->servaddr.sin_addr);
        if (err == 0)
        return UDP_ADDR_INVALID;
    else if (err < 0)
        return UDP_ADDR_FAMILY_INVALID;
    
    client->servaddr.sin_port = htons(port);

    return UDP_OK;
}

udp_err_t udp_send(const void* buf, size_t n, udp_t* session)
{
    if (sendto(session->fd, buf, n, MSG_CONFIRM, (const struct sockaddr *)&session->servaddr, sizeof(session->servaddr)) < 0)
        return UDP_SEND_FAIL;
}

udp_err_t udp_recv(void* buf, size_t n, udp_t* session)
{
    if (recvfrom(session->fd, buf, n, MSG_WAITALL, (struct sockaddr *)&session->servaddr, sizeof(session->servaddr)) < 0)
        return UDP_RECV_FAIL;
}

udp_err_t udp_destroy(udp_t* session)
{
    if (close(session->fd) < 0)
        return UDP_CLOSE_FAIL;

    return UDP_OK;
}