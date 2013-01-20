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

        pthread_t thread;
        pthread_create(&thread, NULL, &worker, &new_fd);
    }

    printf("o_O\n");
}

int uptime()
{
    struct sysinfo info;
    sysinfo(&info);
    return info.uptime;
}

int load()
{
    return 1;
}

void *worker(void *arg)
{
    int *arg_ptr = (int *) arg;
    int sockfd = *arg_ptr;

    char buf[BUF_LEN];
    char temp_buf[BUF_LEN];
    int buf_pos = 0;
    int mistakes = 0;

    for(;;)
    {
        int rcv_result = recv(sockfd, (void *)temp_buf, BUF_LEN, RECV_FLAGS);

        if(rcv_result < 0)
        {
            printf("ERROR: T%d Received -1 bytes\n", sockfd);
            exit(-1);
        }
        else if(mistakes >= MAX_MISTAKES)
        {
            void *msg = (void *) -1;
            send(sockfd, &msg, RESPONSE_SIZE, SEND_FLAGS);
            break;
        }

        strncpy(buf + buf_pos, temp_buf, rcv_result);
        buf_pos += rcv_result;
        printf("[T%d] Message: %s\n", sockfd, buf);

        int request = parse_request(buf, buf_pos);

        void *msg = NULL;
        if(request == REQ_UPTIME)
        {
            printf("[T%d] UPTIME\n", sockfd);
            msg = (void *) uptime();
            clean_buffer(buf);
            buf_pos = 0;
            mistakes = 0;
        }
        else if(request == REQ_LOAD)
        {
            printf("[T%d] LOAD\n", sockfd);
            msg = (void *) load();
            clean_buffer(buf);
            buf_pos = 0;
            mistakes = 0;
        }
        else if(request == REQ_EXIT)
        {
            printf("[T%d] EXIT\n", sockfd);
            msg = (void *) 0;
            clean_buffer(buf);
            buf_pos = 0;
            mistakes = 0;
        }
        else
        {
            printf("[T%d] MISTAKE\n", sockfd);
            msg = (void *) -1;
            mistakes++;
        }

        send(sockfd, &msg, RESPONSE_SIZE, SEND_FLAGS);

        if(request == REQ_EXIT)
            break;
    }

    close(sockfd);
    return NULL;
}

int parse_request(char *buffer, int last_byte)
{
    int i;
    for(i = 0; i < last_byte - 1; i++)
    {
        if(strncmp(buffer + i, "uptime", last_byte - i) == 0)
            return REQ_UPTIME;
        else if(strncmp(buffer + i, "load", last_byte - i) == 0)
            return REQ_LOAD;
        else if(strncmp(buffer + i, "exit", last_byte - i) == 0)
            return REQ_EXIT;
        else if(strncmp(buffer + i, "\x03", last_byte - i) == 0)
            return REQ_EXIT;
    }

    return REQ_GARBAGE;
}

void clean_buffer(char *buffer)
{
    int i;
    for(i = 0; i < BUF_LEN; i++)
        buffer[i] = '\0';
}
