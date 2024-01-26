// Copyright 2023 Saeed Mahmoodi
#include <string.h>

#include "poll_actions.h"


#define SOCK_ADDR_SIZE sizeof(struct sockaddr_in)


int
set_client(struct client_conn *client_conn, int fd,
        struct sockaddr_in *sock_addr) {
    for (int i = 0; i < BACKLOG; i++) {
        if (client_conn[i].pfds.fd == -1) {
            client_conn[i].pfds.fd = fd;
            client_conn[i].client_addr = *sock_addr;
            return 0;
        }
    }
    return -1;
}


int
remove_client(struct client_conn *client_conn, int fd) {
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, SOCK_ADDR_SIZE);

    for (int i = 0; i < BACKLOG; i++) {
        if (client_conn[i].pfds.fd == fd) {
            client_conn[i].pfds.fd = -1;
            client_conn[i].pfds.revents = 0;
            client_conn[i].client_addr = sock_addr;
            return 0;
        }
    }
    return -1;
}


void
init_pollfds(struct client_conn *clients, struct pollfd *pfds) {
    for (int i = 0; i < BACKLOG; i++) {
        pfds[i].fd = clients[i].pfds.fd;
    }
}


int
which_client(struct client_conn *clients, int fd) {
    for (int i = 0; i < BACKLOG; i++) {
        if (clients[i].pfds.fd == fd) {
            return i;
        }
    }
    return -1;
}
