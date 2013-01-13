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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BAD_ARGS    1
#define SOCKET_ERR  2
#define CONNECT_ERR 3

int main(int argc, char **argv)
{
    /* Rough check for valid arguments */
    if(argc != 3)
    {
        printf("usage: mtclient HOST_IP PORT\n");
        exit(BAD_ARGS);
    }

    /* Construct addrinfo structs */
    struct addrinfo *result;
    struct addrinfo hints;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* Exit if unable to get ip info */
    int setup_status = getaddrinfo(argv[1], argv[2], &hints, &result);

    if(setup_status != 0)
    {
        printf("socket setup error: %s\n", gai_strerror(setup_status));
        exit(1);
    }

    /* Exit if unable to acquire socket */
    int sockfd = socket(result->ai_family,
        result->ai_socktype,
        result->ai_protocol);

    if(sockfd < 0)
    {
        printf("ERROR: Could not acquire socket\n");
        exit(SOCKET_ERR);
    }

    /* Exit if unable to acquire port */
    int connect_result = connect(sockfd,
        result->ai_addr,
        result->ai_addrlen);

    if(connect_result < 0)
    {
        printf("ERROR: Could not connect to %s on %s\n", argv[1], argv[2]);
        exit(CONNECT_ERR);
    }

    freeaddrinfo(result);

    return 0;
}

