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

#include "setup_socket.h"
#include "setup_socket.c"
#include "proxy.h"
#include "proxy.c"


void
create_listen_sock(int *main_socket_fd);


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
