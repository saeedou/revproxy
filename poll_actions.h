// Copyright 2023 Saeed Mahmoodi
#ifndef POLL_ACTIONS_H_
#define POLL_ACTIONS_H_


#include <poll.h>
#include <netinet/in.h>


#ifndef BACKLOG
    #define BACKLOG 256
#endif

#ifndef BUFFSIZE
    #define BUFFSIZE 4096
#endif


struct client_conn {
    struct pollfd pfds;
    struct sockaddr_in client_addr;
};


int set_client(struct client_conn *clients, int client_fd,
        struct sockaddr_in *sock_addr);

int remove_client(struct client_conn *clients, int client_fd);

void init_pollfds(struct client_conn *clients, struct pollfd *pfds);

int which_client(struct client_conn *clients, int fd);


#endif  // POLL_ACTIONS_H_
