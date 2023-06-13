// Copyright 2023 Saeed Mahmoodi

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#define BUFFER_SIZE 4096
#define PORT 8002
#define BACKLOG 200
#define IP "127.0.0.1"
#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)


int
main() {
    struct connection {
        int fd;
        int buffsize;
        int addrlen;
        char buff[BUFFER_SIZE];
    };

    int sfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct connection client_conn;
    client_conn.buffsize = BUFFER_SIZE;

    // Create socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        handle_error("Socket creation failed.");
    }

    // Setup address to bind
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Binding address
    if (bind(sfd, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) != 0) {
        handle_error("Binding socket to address failed.");
    }

    // Listening for new connection
    if (listen(sfd, BACKLOG) != 0) {
        handle_error("Listening for new connection failed.");
    }

    // Accepting connection
    client_conn.addrlen = sizeof(client_addr);
    client_conn.fd = accept(sfd, (struct sockaddr *) &client_addr,
            &client_conn.addrlen);

    if (client_conn.fd == -1) {
        handle_error("Accepting connection failed.");
    }

    while (1) {
        bzero(client_conn.buff, client_conn.buffsize);
        read(client_conn.fd, client_conn.buff, client_conn.buffsize);
        write(client_conn.fd, client_conn.buff, client_conn.buffsize);
    }

    close(sfd);
}
