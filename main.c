// Copyright 2023 Saeed Mahmoodi

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>


#define PORT 8011
#define BACKLOG 32
#define IP "127.0.0.1"
#define BUFFER_SIZE 4096
#define handle_error(msg) \
           do {perror(msg); exit(EXIT_FAILURE);} while (0)


struct connection {
    int fd;
    int addrlen;
    char buff[BUFFER_SIZE];
    int buffsize;
    int readbytes;
    int writebytes;
};


void
create_listen_sock(int *main_socket_fd);


int
connect_to_server(char *ip, int port);


int
create_remote_sock(char *ip, int port);


void
proxy(struct connection *clients, int remote_server_fd, int max_client_num,
        fd_set *readfds, fd_set *writefds);


int
main() {
    int main_socket_fd;
    struct connection clients[64];
    struct connection client_conn;
    struct sockaddr_in client_addr;
    struct timeval blocktime;
    int max_client_num;
    int max_fd;
    int fd;
    int i;
    int event;
    int readbytes;
    fd_set readfds;
    fd_set writefds;

    int gport = 8099;
    char gip[] = "127.0.0.1";
    int gsock;

    max_client_num = 64;
    blocktime.tv_sec = 1;
    blocktime.tv_usec = 0;
    for (i = 0; i < max_client_num; i++) {
        clients[i].fd = 0;
        clients[i].buffsize = BUFFER_SIZE;
        memset(clients[i].buff, 0, BUFFER_SIZE);
    }

    create_listen_sock(&main_socket_fd);

    gsock = create_remote_sock(gip, gport);


    while (1) {
        FD_ZERO(&readfds);
        FD_SET(main_socket_fd, &readfds);
        // FD_SET(main_socket_fd, &writefds);
        max_fd = main_socket_fd;

        for (i = 0; i < max_client_num; i++) {
            fd = clients[i].fd;

            if (fd > 0) {
                FD_SET(fd, &readfds);
                // FD_SET(fd, &writefds);
            }

            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        event = select(max_fd + 1, &readfds, NULL, NULL, &blocktime);
        if (event == -1) {
            handle_error("Event loop failed.");
        }

        if (FD_ISSET(main_socket_fd, &readfds)) {
            // Accepting new connection
            client_conn.addrlen = sizeof(client_addr);
            client_conn.fd = accept(main_socket_fd,
                    (struct sockaddr *) &client_addr, &client_conn.addrlen);

            if (client_conn.fd == -1) {
                handle_error("Accepting connection failed.");
            }

            for (i = 0; i < max_client_num; i++) {
                if (clients[i].fd == 0) {
                    clients[i] = client_conn;
                    break;
                }
            }
        }

        proxy(clients, gsock, max_client_num, &readfds, &writefds);
    }

    close(main_socket_fd);
    return EXIT_SUCCESS;
}


void
create_listen_sock(int *main_socket_fd) {
    struct sockaddr_in server_addr;

    *main_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*main_socket_fd == -1) {
        handle_error("Socket creation failed.");
    }

    // Setup address to bind
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Binding address
    if (bind(*main_socket_fd, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) != 0) {
        handle_error("Binding socket to address failed.");
    }

    // Listening for new connection
    if (listen(*main_socket_fd, BACKLOG) != 0) {
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


void
proxy(struct connection *clients, int remote_server_fd, int max_client_num,
        fd_set *readfds, fd_set *writefds) {
    int i;
    int readbytes;

    for (i = 0; i < max_client_num; i++) {
        if (FD_ISSET(clients[i].fd, readfds)) {
            if ((clients[i].readbytes = read(clients[i].fd,
                            clients[i].buff, BUFFER_SIZE)) == 0) {
                close(clients[i].fd);
                clients[i].fd = 0;
                memset(clients[i].buff, 0, BUFFER_SIZE);

            } else {
                write(remote_server_fd, clients[i].buff, clients[i].readbytes);
                readbytes = read(remote_server_fd, clients[i].buff,
                        clients[i].buffsize);
                write(clients[i].fd, clients[i].buff, readbytes);

                close(clients[i].fd);
                clients[i].fd = 0;
                memset(clients[i].buff, 0, BUFFER_SIZE);
            }
        }
    }
}
