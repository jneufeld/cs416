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

    char buffer[BUF_LEN];
    int buf_pos = 0;
    int mistakes = 0;
    int state = REQ_NULL;
    int rcv_result;

    while((rcv_result = recv(sockfd, (void *) buffer, BUF_LEN, RECV_FLAGS)))
    {
        buf_pos += rcv_result;

        int send_msg = 0;
        void *msg = NULL;

        int i;
        for(i = 0; i < buf_pos; i++)
        {
            char ch = buffer[i];

            if(ch == '\x03')
            {
                close(sockfd);
                return NULL;
            }

            switch(state)
            {
                case REQ_NULL:
                    if(ch == 'u')
                        state = REQ_UPTIME_U;
                    else if(ch == 'l')
                        state = REQ_LOAD_L;
                    else if(ch == 'e')
                        state = REQ_EXIT_E;
                    else
                    {
                        mistakes++;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;

                case REQ_UPTIME_U:
                    if(ch == 'p')
                        state = REQ_UPTIME_P;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_UPTIME_P:
                    if(ch == 't')
                        state = REQ_UPTIME_T;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_UPTIME_T:
                    if(ch == 'i')
                        state = REQ_UPTIME_I;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_UPTIME_I:
                    if(ch == 'm')
                        state = REQ_UPTIME_M;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_UPTIME_M:
                    if(ch == 'e')
                    {
                        state = REQ_NULL;
                        msg = (void *) uptime();
                        send_msg = 1;
                        mistakes = 0;
                    }
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;

                case REQ_LOAD_L:
                    if(ch == 'o')
                        state = REQ_LOAD_O;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_LOAD_O:
                    if(ch == 'a')
                        state = REQ_LOAD_A;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_LOAD_A:
                    if(ch == 'd')
                    {
                        state = REQ_NULL;
                        msg = (void *) load();
                        send_msg = 1;
                        mistakes = 0;
                    }
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;

                case REQ_EXIT_E:
                    if(ch == 'x')
                        state = REQ_EXIT_X;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_EXIT_X:
                    if(ch == 'i')
                        state = REQ_EXIT_I;
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
                case REQ_EXIT_I:
                    if(ch == 't')
                    {
                        state = REQ_EXIT;
                        msg = (void *) 0;
                        send_msg = 1;
                    }
                    else
                    {
                        mistakes++;
                        state = REQ_NULL;
                        msg = (void *) -1;
                        send_msg = 1;
                    }
                    break;
            }

            if(send_msg)
            {
                send(sockfd, &msg, RESPONSE_SIZE, SEND_FLAGS);
                send_msg = 0;
            }

            if(state == REQ_EXIT)
            {
                close(sockfd);
                return NULL;
            }

            if(mistakes > MAX_MISTAKES)
            {
                close(sockfd);
                return NULL;
            }
        }

        clean_buffer(buffer);
        buf_pos = 0;
    }

    close(sockfd);
    return NULL;
}


void clean_buffer(char *buffer)
{
    int i;
    for(i = 0; i < BUF_LEN; i++)
        buffer[i] = '\0';
}
