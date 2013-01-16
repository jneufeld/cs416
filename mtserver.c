/*
 * CPSC 416 Assignment 1
 * January 21, 2013
 * 
 * Student Name: Jonathan Neufeld
 * Student Number: 30671093
 * CS ID: p9d8
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#define BAD_ARGS        1
#define SOCKET_ERR      2
#define SOCK_SETUP_ERR  3
#define BIND_ERR        4
#define RECV_FLAGS      0
#define BUF_LEN         16

int main(int argc, char **argv)
{
    /* Rough check for valid arguments */
    if(argc != 3)
    {
        printf("usage: mtserver MAX_CONNECTIONS SERVER_PORT\n");
        exit(BAD_ARGS);
    }

    /* Construct addrinfo structs */
    struct addrinfo *server_info;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* Exit if unable to get ip info */
    int setup_status = getaddrinfo(NULL, argv[2], &hints, &server_info);

    if(setup_status != 0)
    {
        printf("ERROR: Socket setup error (%s)\n", gai_strerror(setup_status));
        exit(SOCK_SETUP_ERR);
    }

    /* Exit if unable to acquire socket */
    int sockfd = socket(server_info->ai_family,
        server_info->ai_socktype,
        server_info->ai_protocol);

    if(sockfd < 0)
    {
        printf("ERROR: Could not acquire socket\n");
        exit(SOCKET_ERR);
    }

    /* Exit if unable to acquire port */
    int bind_result = bind(sockfd,
        server_info->ai_addr,
        server_info->ai_addrlen);

    if(bind_result < 0)
    {
        printf("ERROR: Could not bind socket to port\n");
        exit(BIND_ERR);
    }

    /* Do work ;) */
    int max_clients = atoi(argv[1]);
    listen(sockfd, max_clients);

    struct sockaddr_storage client_sock;
    socklen_t client_addr_sz = sizeof(client_sock);

    for(;;)
    {
        int new_fd = accept(sockfd,
            (struct sockaddr *)&client_sock,
            &client_addr_sz);
        printf("New connection: %d\n", new_fd);

        char buf[BUF_LEN];
        int rcv_result = recv(new_fd, (void *)buf, BUF_LEN, RECV_FLAGS);
        printf("\tRecv length: %d\n\tMessage: %s\n", rcv_result, buf);
    }

    /* Cleanup */
    close(sockfd);
    freeaddrinfo(server_info);

    return 0;
}
