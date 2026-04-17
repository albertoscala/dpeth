#pragma once

#include <cstdlib>
#include <netinet/in.h>
#include <span>

namespace udp
{
    constexpr socklen_t ADDR_LEN = sizeof(sockaddr_in);

    enum class Error
    {
        OK,
        SOCK_FAIL,
        BIND_FAIL
    };

    class Server 
    {
    private:
        int sockfd;
        struct sockaddr_in servaddr{};
    public:
        Server(const char* addr, int port);
        ~Server();

        ssize_t send(std::span<std::byte>& buf);
        ssize_t read(std::span<std::byte>& buf);
    };

    class Client
    {
    private:
        int sockfd;
        struct sockaddr_in servaddr{};
    public:
        Client(const char* addr, int port);
        ~Client();

        ssize_t send(std::span<std::byte>& buf);
        ssize_t read(std::span<std::byte>& buf);
    };
}