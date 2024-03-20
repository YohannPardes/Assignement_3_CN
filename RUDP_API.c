#include "RUDP_API.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


// Allocates a new structure for the RUDP socket
RUDP_Socket* rudp_socket(bool isServer, unsigned short int listen_port)
{
    int reuse = 1;
    RUDP_Socket* sock = (RUDP_Socket*)malloc(sizeof(RUDP_Socket));
    if (sock == NULL)
    {
        fprintf(stderr, "Failed to allocate memory!\n");
        return NULL;
    }

    sock->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock->socket_fd < 0)
    {
        fprintf(stderr, "Failed to create socket!\n");
        free(sock);
        return NULL;
    }

    if (setsockopt(sock->socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        fprintf(stderr, "Failed setsockopt()!\n");
        free(sock);
        return NULL;
    }

    sock->isConnected = false;
    sock->isServer = isServer;

    if (isServer)
    {
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(listen_port);

        if (bind(sock->socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            fprintf(stderr,"Failed bind()");

            free(sock);
            return NULL;
        }

    }
    return sock;
}

// Tries to connect to the other side via RUDP to given IP and port.
int rudp_connect(RUDP_Socket *sockfd, const char *dest_ip, unsigned short int dest_port)
{
    if ((sockfd->isConnected) || (sockfd->isServer))
    {
        fprintf(stderr, "The socket or server is already connected!\n");
        return 0;
    }

    memset(&sockfd->dest_addr, 0, sizeof(sockfd->dest_addr));
    sockfd->dest_addr.sin_family = AF_INET;
    sockfd->dest_addr.sin_addr.s_addr = inet_addr(dest_ip);
    sockfd->dest_addr.sin_port = htons(dest_port);
    sockfd->isConnected = true;
    return 1;
}

int rudp_accept(RUDP_Socket *sockfd)
{
    return 0;
}

int rudp_recv(RUDP_Socket *sockfd, void *buffer, unsigned int buffer_size)
{
    return 0;
}

int rudp_send(RUDP_Socket *sockfd, void *buffer, unsigned int buffer_size)
{
    return 0;
}

// Disconnects from an actively connected socket
int rudp_disconnect(RUDP_Socket *sockfd)
{
    if (!(sockfd->isConnected))
    {
        fprintf(stderr, "The socket is already disconnected\n");
        return 0;
    }
    sockfd->isConnected = false;
    return 1;
}

// This function releases all the memory allocation and resources of the socket.
int rudp_close(RUDP_Socket *sockfd)
{
    if (sockfd == NULL)
    {
        return 0;
    }

    close(sockfd->socket_fd);
    free(sockfd);
    return 1;
}