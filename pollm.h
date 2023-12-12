// Copyright 2023 Saeed Mahmoodi
#ifndef POLLM_H_
#define POLLM_H_


#include <poll.h>
#include <netinet/in.h>

#include "setup_socket.h"


#define BUFFSIZE 4096


struct client_conn {
    struct pollfd pfds;
    char buff[BUFFSIZE];
    struct sockaddr_in client_addr;
};


void set_client(struct client_conn *clients, int client_fd,
        struct sockaddr_in *sock_addr);

void remove_client(struct client_conn *clients, int client_fd);


#endif  // POLLM_H_
