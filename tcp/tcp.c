#include "tcp.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

tcp_err_t tcp_create_server(const tcp_config_t* config, tcp_session_t* session)
{
    session->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (session->listen_fd < 0)
        return TCP_SOCK_FAIL;

    // allow reuse of port after restart
    int reuse = 1;
    if (setsockopt(session->listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        return TCP_SOCKOPT_FAIL;

    memset(&session->addr, 0, sizeof(session->addr));
    session->addr.sin_family      = AF_INET;
    session->addr.sin_port        = htons(config->source_port);
    session->addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(session->listen_fd, (struct sockaddr*)&session->addr, sizeof(session->addr)) < 0)
        return TCP_BIND_FAIL;

    if (listen(session->listen_fd, 1) < 0)
        return TCP_LISTEN_FAIL;

    printf("Server listening on port %d...\n", config->source_port);

    socklen_t peerlen = sizeof(session->peeraddr);
    session->fd = accept(session->listen_fd, (struct sockaddr*)&session->peeraddr, &peerlen);
    if (session->fd < 0)
        return TCP_ACCEPT_FAIL;

    // disable Nagle — we send large frames, no need for buffering
    int flag = 1;
    setsockopt(session->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    printf("Client connected: %s\n", inet_ntoa(session->peeraddr.sin_addr));

    return TCP_OK;
}

tcp_err_t tcp_create_client(const tcp_config_t* config, tcp_session_t* session)
{
    session->listen_fd = -1;

    session->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (session->fd < 0)
        return TCP_SOCK_FAIL;

    // disable Nagle
    int flag = 1;
    setsockopt(session->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    memset(&session->addr, 0, sizeof(session->addr));
    session->addr.sin_family = AF_INET;
    session->addr.sin_port   = htons(config->sink_port);

    if (inet_pton(AF_INET, config->addr, &session->addr.sin_addr) <= 0)
        return TCP_ADDR_INVALID;

    if (connect(session->fd, (struct sockaddr*)&session->addr, sizeof(session->addr)) < 0)
        return TCP_CONNECT_FAIL;

    printf("Connected to %s:%d\n", config->addr, config->sink_port);

    return TCP_OK;
}

tcp_err_t tcp_send(const void* buf, size_t n, tcp_session_t* session)
{
    size_t sent = 0;
    const uint8_t *ptr = (const uint8_t*)buf;

    while (sent < n)
    {
        ssize_t s = send(session->fd, ptr + sent, n - sent, 0);
        if (s <= 0)
            return TCP_SEND_FAIL;
        sent += s;
    }

    return TCP_OK;
}

tcp_err_t tcp_recv(void* buf, size_t n, tcp_session_t* session)
{
    size_t received = 0;
    uint8_t *ptr = (uint8_t*)buf;

    while (received < n)
    {
        ssize_t r = recv(session->fd, ptr + received, n - received, 0);
        if (r <= 0)
            return TCP_RECV_FAIL;
        received += r;
    }

    return TCP_OK;
}

tcp_err_t tcp_destroy(tcp_session_t* session)
{
    if (session->fd >= 0)
        if (close(session->fd) < 0)
            return TCP_CLOSE_FAIL;

    if (session->listen_fd >= 0)
        if (close(session->listen_fd) < 0)
            return TCP_CLOSE_FAIL;

    return TCP_OK;
}