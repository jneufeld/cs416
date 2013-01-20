/*
 * mtserver.c
 *
 * CPSC 416 Assignment 1
 * January 21, 2013
 * 
 * Student Name: Jonathan Neufeld
 * Student Number: 30671093
 * CS ID: p9d8
 */

#include "mtserver.h"

int main(int argc, char **argv)
{
    check_args(argc, argv);
    int sockfd = setup_socket(argv[2]);

    int max_clients = atoi(argv[1]);
    serve(sockfd, max_clients);

    close(sockfd);

    return 0;
}

void check_args(int argc, char **argv)
{
    if(argc != 3)
    {
        printf("usage: mtserver MAX_CONNECTIONS SERVER_PORT\n");
        exit(BAD_ARGS);
    }
}

int setup_socket(char *port)
{
    struct addrinfo *server_info;
    struct addrinfo hints;
    sock_hints_setup(&hints, &server_info, port);

    int sockfd = acquire_socket(server_info->ai_family,
        server_info->ai_socktype,
        server_info->ai_protocol);

    bind_socket(sockfd, server_info->ai_addr, server_info->ai_addrlen);

    return sockfd;
}

void sock_hints_setup(struct addrinfo *hints,
    struct addrinfo **server_info,
    char *port)
{
    init_sock_hints(hints);
    get_sock_hints(hints, server_info, port);
}

void init_sock_hints(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_socktype  = SOCK_STREAM;
    hints->ai_family    = AF_UNSPEC;
    hints->ai_flags     = AI_PASSIVE;
}

void get_sock_hints(struct addrinfo *hints,
    struct addrinfo **server_info,
    char *port)
{
    int setup_status = getaddrinfo(NULL, port, hints, server_info);

    if(setup_status != 0)
    {
        printf("ERROR: Socket setup error (%s)\n", gai_strerror(setup_status));
        exit(SOCK_SETUP_ERR);
    }
}

int acquire_socket(int domain, int type, int protocol)
{
    int sockfd = socket(domain, type, protocol);

    if(sockfd < 0)
    {
        printf("ERROR: Could not acquire socket\n");
        exit(SOCKET_ERR);
    }

    return sockfd;
}

void bind_socket(int sockfd, struct sockaddr *addr, int addrlen)
{
    int bind_result = bind(sockfd, addr, addrlen);

    if(bind_result < 0)
    {
        printf("ERROR: Could not bind socket to port\n");
        exit(BIND_ERR);
    }
}

void serve(int sockfd, int max_clients)
{
    listen(sockfd, max_clients);

    struct sockaddr_storage client_sock;
    socklen_t client_addr_sz = sizeof(client_sock);

    for(;;)
    {
        int new_fd = accept(sockfd,
            (struct sockaddr *)&client_sock,
            &client_addr_sz);
    }

    //char buf[BUF_LEN];
    //char temp_buf[BUF_LEN];
    //int buf_pos = 0;

    //int i;
    //for(i = 0; i < 5; i++)
    //{
    //    /* Receive from client */
    //    int rcv_result = recv(new_fd, (void *)temp_buf, BUF_LEN, RECV_FLAGS);

    //    /* Exit if receive failed */
    //    if(rcv_result < 0)
    //    {
    //        printf("ERROR: Received -1 bytes\n");
    //        exit(-1);
    //    }

    //    /* Copy message into buffer for analysis */
    //    strncpy(buf + buf_pos, temp_buf, rcv_result);
    //    printf("\tRecv length: %d\n\tMessage: %s\n", rcv_result, buf);
    //    buf_pos += rcv_result;

    //    /* Send something back */
    //    void *msg = (void *) uptime();
    //    int send_result = send(new_fd, &msg, RESPONSE_SIZE, SEND_FLAGS);

    //    printf("\tsend_result = %d\n\n", send_result);
    //}
}

int uptime()
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}

void *worker(void *arg)
{
    return NULL;
}
