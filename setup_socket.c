// Copyright 2023 Saeed Mahmoodi
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cutest.h>

#include "setup_socket.h"


#define SOCK_ADDR_SIZE sizeof(struct sockaddr_in)


int
create_listen_sock(int *socket_fd, char *ip, int port) {
    struct sockaddr_in server_addr;

    *socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (*socket_fd == -1) {
        perror("Socket creation failed.");
        return -1;
    }

    // Setup address to bind
    memset(&server_addr, 0, SOCK_ADDR_SIZE);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Binding address
    if (bind(*socket_fd, (struct sockaddr *) &server_addr,
                SOCK_ADDR_SIZE) != 0) {
        perror("Binding socket to address failed.");
        return -1;
    }

    // Listening for new connection
    if (listen(*socket_fd, BACKLOG) != 0) {
        perror("Listening for new connection failed.");
        return -1;
    }
    return 0;
}


int
create_remote_server_client(int *remote_server_sock, char *ip, int port) {
    struct sockaddr_in server_addr;

    if ((*remote_server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed.");
        return -1;
    }

    // Setup address to connect
    memset(&server_addr, 0, SOCK_ADDR_SIZE);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(*remote_server_sock, (struct sockaddr *) &server_addr, \
                SOCK_ADDR_SIZE) != 0) {
        perror("Could not connect to remote server.");
        return -1;
    }
    return 0;
}
