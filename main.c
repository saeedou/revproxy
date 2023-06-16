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


#define BUFFER_SIZE 64
#define PORT 8002
#define BACKLOG 3
#define IP "127.0.0.1"
#define handle_error(msg) \
           do {perror(msg); exit(EXIT_FAILURE);} while (0)


int
main() {
    struct connection {
        int fd;
        int addrlen;
        char buff[BUFFER_SIZE];
        int readbytes;
        int writebytes;
    };

    int main_socket_fd;
    struct connection clients[4];
    struct connection client_conn;
    int max_client_num;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    fd_set readfds;
    fd_set writefds;
    struct timeval blocktime;
    int max_fd;
    int fd;
    int i;
    int event;
    int writebytes;

    max_client_num = 64;
    blocktime.tv_sec = 1;
    blocktime.tv_usec = 0;
    for (i = 0; i < max_client_num; i++) {
        clients[i].fd = 0;
        memset(clients[i].buff, 0, BUFFER_SIZE);
    }

    // Create socket
    main_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket_fd == -1) {
        handle_error("Socket creation failed.");
    }

    // Setup address to bind
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    // Binding address
    if (bind(main_socket_fd, (struct sockaddr *) &server_addr,
                sizeof(server_addr)) != 0) {
        handle_error("Binding socket to address failed.");
    }

    // Listening for new connection
    if (listen(main_socket_fd, BACKLOG) != 0) {
        handle_error("Listening for new connection failed.");
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(main_socket_fd, &readfds);
        FD_SET(main_socket_fd, &writefds);
        max_fd = main_socket_fd;

        for (i = 0; i < max_client_num; i++) {
            fd = clients[i].fd;

            if (fd > 0) {
                FD_SET(fd, &readfds);
                FD_SET(fd, &writefds);
                FD_SET(10, &writefds);
            }

            if (fd > max_fd) {
                max_fd = fd;
            }
        }

        // event = select(max_fd + 1, &readfds, &writefds, NULL, &blocktime);
        event = select(max_fd + 1, &readfds, NULL, NULL, &blocktime);
        if (event == -1) {
            handle_error("Event loop failed.");
        }

        if (FD_ISSET(main_socket_fd, &readfds)) {
            // Accepting connection
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

        for (i = 0; i < max_client_num; i++) {
            if (FD_ISSET(clients[i].fd, &readfds)) {
                if ((clients[i].readbytes = read(clients[i].fd,
                                clients[i].buff, BUFFER_SIZE)) == 0) {
                    close(clients[i].fd);
                    clients[i].fd = 0;
                    memset(clients[i].buff, 0, BUFFER_SIZE);
                } else {
                    clients[i].writebytes = write(clients[i].fd,
                            clients[i].buff, clients[i].readbytes);
                    clients[i].writebytes += writebytes;

                    if (clients[i].readbytes == clients[i].writebytes) {
                        close(clients[i].fd);
                        clients[i].fd = 0;
                        memset(clients[i].buff, 0, BUFFER_SIZE);
                    }
                }
            }
        }
    }

    close(main_socket_fd);
    return EXIT_SUCCESS;
}
