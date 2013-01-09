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

#define BAD_ARGS 1

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("usage: mtserver MAX_CONNECTIONS SERVER_PORT\n");
        exit(BAD_ARGS);
    }

    int connections = atoi(argv[1]);
    int server_port = atoi(argv[2]);

    printf("MAX_CONNECTIONS=%d\nSERVER_PORT=%d\n", connections, server_port);

    return 0;
}
