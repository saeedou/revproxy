// Copyright 2023 Saeed Mahmoodi

#include "setup_socket.h"


void
create_listen_sock(int *socket_fd) {
    struct sockaddr_in server_addr;

    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd == -1) {
        handle_error("Socket creation failed.");
    }

    // Setup address to bind
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

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


int
create_remote_sock(char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Creating socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connecting failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}
