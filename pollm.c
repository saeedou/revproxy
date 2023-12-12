// Copyright 2023 Saeed Mahmoodi
#include "pollm.h"


#define SOCK_ADDR_SIZE sizeof(struct sockaddr_in)

void
set_client(struct client_conn *client_conn, int fd,
        struct sockaddr_in *sock_addr) {
    for (int i = 0; i < BACKLOG; i++) {
        if (client_conn[i].pfds.fd == -1) {
            client_conn[i].pfds.fd = fd;
            client_conn[i].client_addr = *sock_addr;
            break;
        }
    }
}


void
remove_client(struct client_conn *client_conn, int fd) {
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, SOCK_ADDR_SIZE);

    for (int i = 0; i < BACKLOG; i++) {
        if (client_conn[i].pfds.fd == fd) {
            client_conn[i].pfds.fd = -1;
            client_conn[i].client_addr = sock_addr;
            break;
        }
    }
}
