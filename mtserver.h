/*
 * mtserver.h
 *
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
#include <sys/sysinfo.h>
#include <pthread.h>

#define BAD_ARGS        1
#define SOCKET_ERR      2
#define SOCK_SETUP_ERR  3
#define BIND_ERR        4

#define RECV_FLAGS      0
#define SEND_FLAGS      0
#define BUF_LEN         64
#define RESPONSE_SIZE   4

#define REQ_UPTIME      1
#define REQ_LOAD        2
#define REQ_EXIT        3

void check_args(int argc, char **argv);
int setup_socket(char *port);
void sock_hints_setup(struct addrinfo *hints,
    struct addrinfo **server_info,
    char *port);
void init_sock_hints(struct addrinfo *hints);
void get_sock_hints(struct addrinfo *hints,
    struct addrinfo **server_info,
    char *port);
int acquire_socket(int domain, int type, int protocol);
void bind_socket(int sockfd, struct sockaddr *addr, int addrlen);
void serve(int sockfd, int max_clients);
void *worker(void *arg);
int uptime();
int parse_request(char *buffer, int last_byte);
