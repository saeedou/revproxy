// Copyright 2023 Saeed Mahmoodi
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cutest.h>

#include "setup_socket.h"


#define handle_error(msg) \
           do {perror(msg); exit(EXIT_FAILURE);} while (0)


void
create_listen_sock(int *socket_fd, char *ip, int *port) {
    struct sockaddr_in server_addr;

    *socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (*socket_fd == -1) {
        handle_error("Socket creation failed.");
    }

    // Setup address to bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    // Binding address
    if (bind(*socket_fd, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) != 0) {
        handle_error("Binding socket to address failed.");
    }

    // Listening for new connection
    if (listen(*socket_fd, BACKLOG) != 0) {
        handle_error("Listening for new connection failed.");
    }
}
