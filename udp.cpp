#include "udp.hpp"


#include <sys/socket.h>
#include <arpa/inet.h>

udp::Server::Server(const char* addr, int port)
{
    //TODO: Aggiungere connect (ping il client con config, prima risposta connettiti)

    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) 
    { 
        //TODO: Implement error handling 
    } 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    inet_pton(AF_INET, "192.168.1.100", &servaddr.sin_addr); //TODO: Handle error 
    servaddr.sin_port = htons(port);
      
    // Bind the socket with the server address 
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        //TODO: Implement error handling
    }
}

udp::Client::Client(const char* addr, int port)
{
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        //TODO: Implement error handling
    }

    // Filling server information 
    servaddr.sin_family = AF_INET; // IPv4 
    inet_pton(AF_INET, "192.168.1.100", &servaddr.sin_addr); //TODO: Handle error 
    servaddr.sin_port = htons(port);
}

ssize_t udp::Server::send(std::span<std::byte>& buf)
{
    return sendto(sockfd, buf.data(), buf.size_bytes(), MSG_CONFIRM, (const struct sockaddr *)&servaddr, ADDR_LEN);
}

ssize_t udp::Client::send(std::span<std::byte>& buf)
{
    return sendto(sockfd, buf.data(), buf.size_bytes(), MSG_CONFIRM, (const struct sockaddr *)&servaddr, ADDR_LEN);
}

ssize_t udp::Server::read(std::span<std::byte>& buf)
{
    return recvfrom(sockfd, buf.data(), buf.size_bytes(), MSG_WAITALL, (struct sockaddr *)&servaddr, NULL);
}

ssize_t udp::Client::read(std::span<std::byte>& buf)
{
    return recvfrom(sockfd, buf.data(), buf.size_bytes(), MSG_WAITALL, (struct sockaddr *)&servaddr, NULL);
}